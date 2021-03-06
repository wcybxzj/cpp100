﻿#ifndef _CELL_SERVER_HPP_
#define _CELL_SERVER_HPP_

#include "CELL.hpp"
#include "CELLClient.hpp"
#include "INetEvent.hpp"
#include "CELLThread.hpp"

class CellServer {
private:
	//正式客户队列
	std::map<SOCKET, CellClient*> _clients;
	//缓冲客户队列
	std::vector<CellClient*> _clientsBuff;

	//
	CellTaskServer _taskServer;

	std::mutex _mutex;
	INetEvent* _pNetEvent;

	fd_set _fdRead_bak;
	bool _clients_change = true;
	SOCKET _maxSock;

	int _id = -1;

	time_t _oldTime = CELLTime::getNowInMilliSec();
	CELLThread _thread;

public:
	CellServer(int id)
	{
		_id = id;
		_pNetEvent = nullptr;
		_taskServer.serverId = id;
	}

	~CellServer()
	{
		printf("~CellServer() cellserverid:%d  begin\n", _id);
		Close();
		printf("~CellServer() cellserverid:%d  end\n", _id);
	}

	void setEventObj(INetEvent* event)
	{
		_pNetEvent = event;
	}

	//关闭socket
	void Close() {
		printf("CellServer->Close() cellserverid:%d  begin\n", _id);
		_taskServer.Close();
		_thread.Close();
		printf("CellServer->Close() cellserverid:%d  end\n", _id);
	}

	//处理网络请求
	void OnRun(CELLThread*pThread) {
		while (pThread->isRun())
		{
			 //printf("CellServer()-> OnRun()\n");
			if (_clientsBuff.size() > 0)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pClient : _clientsBuff)
				{
					_clients[pClient->sockfd()] = pClient;
					pClient->serverId = _id;
					if (_pNetEvent)
					{
						_pNetEvent->OnNetJoin(pClient);
					}
				}
				_clientsBuff.clear();
				_clients_change = true;
			}

			//如果没有需要处理的客户端，就跳过
			if (_clients.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				//必须
				_oldTime = CELLTime::getNowInMilliSec();
				continue;
			}

			fd_set fdRead;
			fd_set fdWrite;

			FD_ZERO(&fdRead);

			//优化1:减少FS_SET次数
			if (_clients_change)
			{
				//将描述符（socket）加入集合
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
			memcpy(&fdWrite, &_fdRead_bak, sizeof(fd_set));

			//printf("before:_clients.size:%d, fdRead.fd_count:%d\n", _clients.size(), fdRead.fd_count);

			timeval t = { 0,1 };
			int ret = select(_maxSock + 1, &fdRead, &fdWrite, nullptr, &t);//阻塞

			if (ret < 0)
			{
				pThread->Exit();
				printf("CellServer() select < 0  error!!!\n");
				break;
			}

			RecvData(fdRead);
			//WriteData(fdWrite);
			CheckTime();
		}
		printf("CellServer cellserverid:%d.OnRun exit\n", _id);
	}

	void CheckTime()
	{
		auto nowTime = CELLTime::getNowInMilliSec();
		auto dt = nowTime - _oldTime;
		_oldTime = nowTime;
		for (auto iter = _clients.begin(); iter!=_clients.end(); )
		{
			if (iter->second->checkHeart(dt))
			{
				if (_pNetEvent)
				{
					_pNetEvent->OnNetLeave(iter->second);
				}
				_clients_change = true;
				delete iter->second;
				auto iterOld = iter;
				iter++;
				_clients.erase(iterOld);
			}
			else {
				//定时发送
				iter->second->checkSend(dt);
				iter++;
			}
		}
	}
	void OnClientLeave(CellClient* pClient)
	{
		if (_pNetEvent)
			_pNetEvent->OnNetLeave(pClient);
		_clients_change = true;
		delete pClient;
	}

	void WriteData(fd_set& fdWrite)
	{

#ifdef _WIN32
		for (int i = 0; i < fdWrite.fd_count; i++)
		{
			auto iter = _clients.find(fdWrite.fd_array[i]);
			if (iter != _clients.end())
			{
				if (-1 == iter->second->SendDataReal())
				{
					OnClientLeave(iter->second);
					_clients.erase(iter);
				}
			}
		}
#else
		for (auto iter = _clients.begin(); iter != _clients.end(); )
		{
			if (FD_ISSET(iter->second->sockfd(), &fdWrite))
			{
				if (-1 == iter->second->SendDataReal())
				{
					OnClientLeave(iter->second);
					auto iterOld = iter;
					iter++;
					_clients.erase(iterOld);
					continue;
				}
			}
			iter++;
		}
#endif

	}

	void RecvData(fd_set&fdRead)
	{
		//优化2:_clients从vector换成map,放弃FD_ISET去判断fd是否有事件
#ifdef _WIN32
		for (int n = 0; n < fdRead.fd_count; n++)
		{
			auto iter = _clients.find(fdRead.fd_array[n]);
			if (iter != _clients.end())
			{
				if (-1 == RecvData(iter->second))
				{
					OnClientLeave(iter->second);
					_clients.erase(iter->first);
				}
			}
		}

#else
		for (auto iter = _clients.begin(); iter != _clients.end(); )
		{
			if (FD_ISSET(iter->second->sockfd(), &fdRead))
			{
				if (-1 == RecvData(iter->second))
				{
					OnClientLeave(iter->second);
					auto iterOld = iter;
					iter++;
					_clients.erase(iterOld);
					continue;
				}
			}
			iter++;
		}
#endif
		//printf("空闲时间处理其它业务..\n");
	}


	//接受数据 处理粘包 拆分包
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
				//printf("消息缓冲区中的消息不足够一条完成的消息");
				break;
			}
		}
		return 0;
	}

	//响应网络请求
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

	//void addSendTask(CellClient* pClient, netmsg_DataHeader* header) {
	//	_taskServer.addTask([pClient, header]() {
	//		pClient->SendData(header);
	//		delete header;
	//	});
	//}

	void Start() {
		_taskServer.Start();
		_thread.Start(
			nullptr,
			[this](CELLThread* pThread) {
				OnRun(pThread);
			},
			[this](CELLThread* pThread) {
				ClearClients();
			}
		);
	}

	size_t getClientCount() {
		return _clients.size() + _clientsBuff.size();
	}

	private:
		void ClearClients()
		{
			for (auto iter : _clients)
			{
				delete iter.second;
			}
			_clients.clear();

			for (auto iter : _clientsBuff)
			{
				delete iter;
			}
			_clientsBuff.clear();
		}



};

#endif