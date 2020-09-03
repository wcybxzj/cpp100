#ifndef _CELL_SERVER_HPP_
#define _CELL_SERVER_HPP_

#include "CELL.hpp"
#include "CELLClient.hpp"
#include "INetEvent.hpp"


class CellSendMsg2ClientTask :public CellTask
{
private:
	CellClient* _pClient;
	netmsg_DataHeader* _pHeader;

public:
	CellSendMsg2ClientTask(CellClient* pClient,
		netmsg_DataHeader* header)
	{
		_pClient = pClient;
		_pHeader = header;
	}

	void doTask() {
		_pClient->SendData(_pHeader);
		delete _pHeader;
	}
};

class CellServer {
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

	//�ر�socket
	void Close() {
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				closesocket(_clients[n]->sockfd());
				delete _clients[n];
			}
#else
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				close(_clients[n]->sockfd());
				delete _clients[n];
			}
#endif
			_clients.clear();
		}
	}

	//�Ƿ�����
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	fd_set _fdRead_bak;
	bool _clients_change;
	SOCKET _maxSock;

	//������������
	bool OnRun() {
		_clients_change = true;
		while (isRun())
		{
			if (_clientsBuff.size() > 0)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pClient : _clientsBuff)
				{
					_clients[pClient->sockfd()] = pClient;
				}
				_clientsBuff.clear();
				_clients_change = true;
			}

			//���û����Ҫ����Ŀͻ��ˣ�������
			if (_clients.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}

			fd_set fdRead;
			FD_ZERO(&fdRead);

			//�Ż�1:����FS_SET����
			if (_clients_change)
			{
				//����������socket�����뼯��
				_maxSock = _clients.begin()->second->sockfd();
				for (auto iter : _clients)
				{
					FD_SET(iter.second->sockfd(), &fdRead);
					if (_maxSock < iter.second->sockfd())
					{
						_maxSock = iter.second->sockfd();
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
			int ret = select(_maxSock + 1, &fdRead, nullptr, nullptr, nullptr);//����

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

			//�Ż�2:_clients��vector����map,����FD_ISETȥ�ж�fd�Ƿ����¼�
#ifdef _WIN32
			for (int n = 0; n < fdRead.fd_count; n++)
			{
				auto iter = _clients.find(fdRead.fd_array[n]);
				if (iter != _clients.end())
				{
					if (-1 == RecvData(iter->second))
					{
						if (_pNetEvent)
							_pNetEvent->OnNetLeave(iter->second);
						_clients_change = true;
						_clients.erase(iter->first);
					}
				}
				else {
					printf("error. if (iter == _clients.end())\n");
				}
			}

#else
			std::vector<CellClientPtr> temp;
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
			}
#endif


			//printf("����ʱ�䴦������ҵ��..\n");
		}
	}

	//�������� ����ճ�� ��ְ�
	int RecvData(CellClient* pClient) {
		char* _szRecv = pClient->msgBuf() + pClient->getLastPos();
		int nLen = (int)recv(pClient->sockfd(), _szRecv, (RECV_BUFF_SIZE)-pClient->getLastPos(), 0);

		_pNetEvent->OnNetRecv(pClient);

		if (nLen <= 0)
		{
			return -1;
		}

		pClient->setLastPos(pClient->getLastPos() + nLen);

		while (pClient->getLastPos() >= sizeof(netmsg_DataHeader))
		{
			netmsg_DataHeader* header = (netmsg_DataHeader*)pClient->msgBuf();
			if (pClient->getLastPos() >= header->dataLength)
			{
				int nSize = pClient->getLastPos() - header->dataLength;
				OnNetMsg(pClient, header);
				memcpy(pClient->msgBuf(), pClient->msgBuf() + (header->dataLength), nSize);
				pClient->setLastPos(nSize);
			}
			else
			{
				//printf("��Ϣ�������е���Ϣ���㹻һ����ɵ���Ϣ");
				break;
			}
		}
		return 0;
	}

	//��Ӧ��������
	virtual void OnNetMsg(CellClient* pClient, netmsg_DataHeader* header)
	{
		_pNetEvent->OnNetMsg(this, pClient, header);
	}

	void addClient(CellClient* pClient)
	{
		std::lock_guard<std::mutex>lock(_mutex);
		//_mutex.lock();
		_clientsBuff.push_back(pClient);
		//_mutex.unlock();
	}

	void addSendTask(CellClient* pClient, netmsg_DataHeader* header) {
		CellSendMsg2ClientTask* task = new CellSendMsg2ClientTask(pClient, header);
		_taskServer.addTask(task);
	}

	void Start() {
		//������Ĭ�ϵ�һ�������Ǹ�this
		//_thread = std::thread t(&CellServer::OnRun, this);
		_thread = std::thread(std::mem_fn(&CellServer::OnRun), this);
		_taskServer.Start();
	}

	size_t getClientCount() {
		return _clients.size() + _clientsBuff.size();
	}

private:
	SOCKET _sock;
	////��ʽ�ͻ�����
	//std::vector<ClientSocket*> _clients;//�����ö��ڴ� c++ջ�ڴ�̫С
	////����ͻ�����
	//std::vector<ClientSocket*> _clientsBuff;

	//��ʽ�ͻ�����
	std::map<SOCKET, CellClient*> _clients;
	//����ͻ�����
	std::vector<CellClient*> _clientsBuff;

	//
	CellTaskServer _taskServer;

	std::mutex _mutex;
	std::thread _thread;
	INetEvent* _pNetEvent;
};

#endif