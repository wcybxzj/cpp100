#ifndef _CELL_SERVER_HPP_
#define _CELL_SERVER_HPP_

#include "CELL.hpp"
#include "CELLClient.hpp"
#include "INetEvent.hpp"


class CellSendMsg2ClientTask :public CellTask
{

	CellClientPtr _pClient;
	netmsg_DataHeaderPtr _pHeader;

public:
	CellSendMsg2ClientTask(CellClientPtr& pClient, netmsg_DataHeaderPtr& header)
	{
		_pClient = pClient;
		_pHeader = header;
	}

	void doTask() {
		_pClient.get()->SendData(_pHeader);
	}
};

class CellServer {
private:
	SOCKET _sock;
	//正式客户队列
	std::map<SOCKET, CellClientPtr> _clients;
	//缓冲客户队列
	std::vector<CellClientPtr> _clientsBuff;
	//
	CellTaskServer _taskServer;

	std::mutex _mutex;
	std::thread _thread;
	INetEvent* _pNetEvent;

public:
	CellServer(SOCKET sock = INVALID_SOCKET)
	{
		_sock = sock;
		_pNetEvent = nullptr;
	}

	~CellServer()
	{
		Close();
		_sock = INVALID_SOCKET;
	}

	void setEventObj(INetEvent* event)
	{
		_pNetEvent = event;
	}

	//关闭socket
	void Close() {
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				closesocket(_clients[n].get()->sockfd());
			}
#else
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				close(_clients[n].get()->sockfd());
			}
#endif
			_clients.clear();
		}
	}

	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	fd_set _fdRead_bak;
	bool _clients_change;
	SOCKET _maxSock;

	//处理网络请求
	bool OnRun() {
		_clients_change = true;
		while (isRun())
		{
			if (_clientsBuff.size() > 0)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pClient : _clientsBuff)
				{
					_clients[pClient.get()->sockfd()] = pClient;
				}
				_clientsBuff.clear();
				_clients_change = true;
			}

			//如果没有需要处理的客户端，就跳过
			if (_clients.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}

			fd_set fdRead;
			FD_ZERO(&fdRead);

			//优化1:减少FS_SET次数
			if (_clients_change)
			{
				//将描述符（socket）加入集合
				_maxSock = _clients.begin()->second.get()->sockfd();
				for (auto iter : _clients)
				{
					FD_SET(iter.second.get()->sockfd(), &fdRead);
					if (_maxSock < iter.second.get()->sockfd())
					{
						_maxSock = iter.second.get()->sockfd();
					}
				}
				memcpy(&_fdRead_bak, &fdRead, sizeof(fd_set));
			}
			else
			{
				memcpy(&fdRead, &_fdRead_bak, sizeof(fd_set));
			}

			//printf("before:_clients.size:%d, fdRead.fd_count:%d\n", _clients.size(), fdRead.fd_count);

			timeval t1 = { 0,0 };
			int ret = select(_maxSock + 1, &fdRead, nullptr, nullptr, nullptr);//阻塞

			if (ret < 0)
			{
				Close();
				printf("CellServer() select < 0  error!!!\n");
				return false;
			}
			else if (ret == 0)
			{
				continue;
			}

			//优化2:_clients从vector换成map,放弃FD_ISET去判断fd是否有事件
#ifdef _WIN32
			for (int n = 0; n < fdRead.fd_count; n++)
			{
				auto iter = _clients.find(fdRead.fd_array[n]);
				if (iter != _clients.end())
				{
					if (-1 == RecvData(iter->second))
					{
						if (_pNetEvent) {
							_pNetEvent->OnNetLeave(iter->second);
						}
						
						_clients_change = true;
						_clients.erase(iter->first);
					}
				}
				else {
					printf("error. if (iter == _clients.end())\n");
				}
			}

#else
			std::vector<ClientSocket*> temp;
			for (auto iter : _clients)
			{
				if (FD_ISSET(iter.second->sockfd(), &fdRead))
				{
					if (-1 == RecvData(iter.second))
					{
						if (_pNetEvent)
							_pNetEvent->OnNetLeave(iter.second);
						_clients_change = false;
						temp.push_back(iter.second);
					}
				}
			}
			for (auto pClient : temp)
			{
				_clients.erase(pClient->sockfd());
				delete pClient;
			}
#endif


			//printf("空闲时间处理其它业务..\n");
		}
	}

	//接受数据 处理粘包 拆分包
	int RecvData(CellClientPtr& pClient) {
		char* _szRecv = pClient.get()->msgBuf() + pClient.get()->getLastPos();
		int nLen = (int)recv(pClient.get()->sockfd(), _szRecv, (RECV_BUFF_SIZE)-pClient.get()->getLastPos(), 0);

		_pNetEvent->OnNetRecv(pClient);

		if (nLen <= 0)
		{
			return -1;
		}

		pClient.get()->setLastPos(pClient.get()->getLastPos() + nLen);

		while (pClient.get()->getLastPos() >= sizeof(netmsg_DataHeader))
		{
			netmsg_DataHeader* header = (netmsg_DataHeader*)pClient.get()->msgBuf();
			if (pClient.get()->getLastPos() >= header->dataLength)
			{
				int nSize = pClient.get()->getLastPos() - header->dataLength;
				OnNetMsg(pClient, header);
				memcpy(pClient.get()->msgBuf(), pClient.get()->msgBuf() + (header->dataLength), nSize);
				pClient.get()->setLastPos(nSize);
			}
			else
			{
				//printf("消息缓冲区中的消息不足够一条完成的消息");
				break;
			}
		}
		return 0;
	}

	//响应网络请求
	virtual void OnNetMsg(CellClientPtr& pClient, netmsg_DataHeader* header)
	{
		_pNetEvent->OnNetMsg(this, pClient, header);
	}

	void addClient(CellClientPtr& pClient)
	{
		std::lock_guard<std::mutex>lock(_mutex);
		//_mutex.lock();
		_clientsBuff.push_back(pClient);
		//_mutex.unlock();
	}
											 
	void addSendTask(CellClientPtr& pClient, std::shared_ptr<netmsg_DataHeader>& header) {
		auto task = std::make_shared<CellSendMsg2ClientTask>(pClient, header);
		_taskServer.addTask((CellTaskPtr)task);

	}

	void Start() {
		//对象函数默认第一个参数是个this
		//_thread = std::thread t(&CellServer::OnRun, this);
		_thread = std::thread(std::mem_fn(&CellServer::OnRun), this);
		_taskServer.Start();
	}

	size_t getClientCount() {
		return _clients.size() + _clientsBuff.size();
	}


};

typedef std::shared_ptr<CellServer> CellServerPtr;

#endif