#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#include "CELL.hpp"
#include "INetEvent.hpp"

class ClientSocket
{
private:
	//消息缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE * 5];//100KB
	int _lastPos = 0;
	SOCKET _sockfd;

public:
	ClientSocket(SOCKET sockfd=INVALID_SOCKET) {
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, sizeof(_szMsgBuf));
		_lastPos = 0;
	}

	SOCKET sockfd() {
		return _sockfd;
	}

	char* msgBuf() {
		return _szMsgBuf;
	}

	int getLastPos() {
		return _lastPos;
	}

	void setLastPos(int pos) {
		_lastPos = pos;
	}

	~ClientSocket() {
#ifdef _WIN32
		closesocket(_sockfd);
#else
		close(_sockfd);
#endif
	}

	//发送指定socket数据
	int SendData(DataHeader* header)
	{
		int ret = SOCKET_ERROR;
		if (header)
		{
			ret = send(_sockfd, (const char*)header, header->dataLength, 0);
			if (ret == SOCKET_ERROR)
			{
				printf("send error!\n");
			}
		}
		return ret;
	}

};

class INetEvent {
public:
	virtual void OnNetLeave(ClientSocket* pClient) = 0;

	virtual void OnNetMsg(ClientSocket* pClient, DataHeader *header) = 0;

	virtual void OnNetJoin(ClientSocket* pClient) = 0;

	virtual void OnNetRecv(ClientSocket* pClient) = 0;
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

	//关闭socket
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

	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
		//return _sock != INVALID_SOCKET && _isConnect;
	}

	//处理网络请求
	bool OnRun() {
		SOCKET _cSock;
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

			//将描述符（socket）加入集合
			SOCKET _maxSock = _clients.begin()->second->sockfd();
			for (auto iter : _clients)
			{
				FD_SET(iter.second->sockfd(), &fdRead);
				if (_maxSock < iter.second->sockfd())
				{
					_maxSock = iter.second->sockfd();
				}
			}

			//printf("before:_clients.size:%d, fdRead.fd_count:%d\n", _clients.size(), fdRead.fd_count);

			timeval t1 = { 0,10 };
			int ret = select(_maxSock + 1, &fdRead, nullptr, nullptr, nullptr);//阻塞

			if (ret < 0)
			{
				Close();
				printf("select < 0  error!!!\n");
				return false;
			}
			else if (ret == 0)
			{
				continue;
			}

			for (int n = 0; n < fdRead.fd_count; n++)
			{
				auto iter = _clients.find(fdRead.fd_array[n]);
				if (iter != _clients.end())
				{
					if (-1 == RecvData(iter->second))
					{
						if (_pNetEvent)
							_pNetEvent->OnNetLeave(iter->second);
						//_clients_change = true;
						_clients.erase(iter->first);
					}
				}
				else {
					printf("error. if (iter != _clients.end())\n");
				}
			}
			//printf("空闲时间处理其它业务..\n");
		}
	}

//	char _szRecv[RECV_BUFF_SIZE] = {};
	//接受数据 处理粘包 拆分包
	int RecvData(ClientSocket* pClient) {

		char* _szRecv = pClient->msgBuf() + pClient->getLastPos();
		int nLen = (int)recv(pClient->sockfd(), _szRecv, (RECV_BUFF_SIZE)-pClient->getLastPos(), 0);
		_pNetEvent->OnNetRecv(pClient);

		if (nLen <= 0)
		{
			//if (nLen == 0)
			//{
			//	printf("recv nLen:%d\n", nLen);
			//}
			//else {
			//	printf("recv nLen:%d\n", nLen);
			//}


			//printf("client closed!fd:%d\n", pClient->sockfd());
			return -1;
		}

		//memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, nLen);
		pClient->setLastPos(pClient->getLastPos() + nLen);

		while (pClient->getLastPos() >= sizeof(DataHeader))
		{
			DataHeader* header = (DataHeader*)pClient->msgBuf();
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
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header)
	{
		_pNetEvent->OnNetMsg(pClient, header);
	}

	void addClient(ClientSocket* pClient)
	{
		std::lock_guard<std::mutex>lock(_mutex);
		//_mutex.lock();
		_clientsBuff.push_back(pClient);
		//_mutex.unlock();
	}

	void Start() {
		//对象函数默认第一个参数是个this
		//_thread = std::thread t(&CellServer::OnRun, this);
		_thread = std::thread(std::mem_fn(&CellServer::OnRun), this);
	}

	size_t getClientCount() {
		return _clients.size() + _clientsBuff.size();
	}

private:
	SOCKET _sock;
	////正式客户队列
	//std::vector<ClientSocket*> _clients;//尽量用堆内存 c++栈内存太小
	////缓冲客户队列
	//std::vector<ClientSocket*> _clientsBuff;

	//正式客户队列
	std::map<SOCKET, ClientSocket*> _clients;
	//缓冲客户队列
	std::vector<ClientSocket*> _clientsBuff;


	std::mutex _mutex;
	std::thread _thread;
	INetEvent* _pNetEvent;
};

class EasyTcpServer : INetEvent
{
private:
	SOCKET _sock;
	std::vector<CellServer*> _cellServers;
	CELLTimestamp _tTime;

protected:
	std::atomic_int _recvCount;

	//收到消息计数
	std::atomic_int _msgCount;
	//客户端计数
	std::atomic_int _clientCount;

public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
		_msgCount = 0;
		_clientCount = 0;
		_recvCount = 0;
	}

	virtual ~EasyTcpServer()
	{
		Close();
	}

	//初始化Socket
	SOCKET InitSocket()
	{
#ifdef _WIN32
		//启动Windows socket 2.x环境
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		if (INVALID_SOCKET != _sock)
		{
			printf("<socket=%d>关闭旧连接...\n", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			printf("错误，建立Socket失败...\n");
		}
		else {
			//printf("建立Socket=<%d>成功...\n", _sock);
		}
#ifndef _WIN32
		int one=1;
		if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) {
			Close();
		}
#endif
		return _sock;
	}

	//绑定ip和port
	int Bind(const char* ip, unsigned short port) {

		if (_sock == INVALID_SOCKET)
		{
			InitSocket();
		}

		//bind
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);

#ifdef _WIN32
		if (ip) {
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else {
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
		}
#else
		if (ip) {
			_sin.sin_addr.s_addr = inet_addr(ip);
		}
		else {
			_sin.sin_addr.s_addr = INADDR_ANY;
		}
#endif
		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(_sin));
		if (SOCKET_ERROR == ret)
		{
			printf("错误,绑定网络端口<%d>失败...\n", port);
		}
		else {
			printf("绑定端口成功, ok, port:<%d>  !!!!!\n", port);
		}
		return ret;
	}

	//监听端口号
	int Listen(int n) {
		//listen
		int ret= listen(_sock, n);
		if (SOCKET_ERROR == ret)
		{
			printf("socket=<%d>错误,监听的网络端口失败\n", (int)_sock);
		}
		else {
			printf("socket=<%d>监听网络端口成功...\n", (int)_sock);
		}
		return ret;
	}
	
	//接受客户端连接
	SOCKET Accept() {
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET cSock = INVALID_SOCKET;
		
#ifdef _WIN32
		cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif

		if (INVALID_SOCKET == cSock) 
		{
			printf("socket=<%d>,错误, 接收到无效SOCKET... \n",(int)_sock);
		}
		else 
		{
			addClientToCellServer(new ClientSocket(cSock));
			//inet_ntoa(clientAddr.sin_addr);// client ip
		}
		return cSock;
	}

	void addClientToCellServer(ClientSocket* pClient) {
		auto pMinServer = _cellServers[0];
		//加入到客户最少的CellServer
		for (auto pCellServer:_cellServers)
		{
			if (pMinServer->getClientCount() > pCellServer->getClientCount())
			{
				pMinServer = pCellServer;
			}
		}
		pMinServer->addClient(pClient);
		OnNetJoin(pClient);
	}

	void Start(int nCellServer) {
		for (int  i= 0; i < nCellServer; i++)
		{
			auto ser = new CellServer(_sock);
			_cellServers.push_back(ser);
			ser->setEventObj(this);
			ser->Start();
		}
	}

	//关闭socket
	void Close() {
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(_sock);
			//清除Windows socket环境
			WSACleanup();
#else
			// 8 关闭套节字closesocket
			close(_sock);
#endif
		}
	}

	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
		//return _sock != INVALID_SOCKET && _isConnect;
	}

	//处理网络请求
	bool OnRun() {
		SOCKET _cSock;
		if (isRun())
		{
			time4msg();

			fd_set fdRead;
			fd_set fdWrite;
			fd_set fdExp;

			FD_ZERO(&fdRead);

			FD_SET(_sock, &fdRead);

			timeval t = { 0,10 };

			int select_ret = select(_sock + 1, &fdRead,0 ,0 , &t);//非阻塞select
			//int select_ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, NULL);//阻塞
			//printf("select_count:%d, select_ret:%d\n", select_count, select_ret);

			if (select_ret < 0)
			{
				Close();
				printf("select < 0  error!!!\n");
				return false;
			}

			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				_cSock = Accept();
				if (INVALID_SOCKET == _cSock) {
					printf("accept errror!!!\n");
				}
			}

			//printf("空闲时间处理其它业务..\n");
			return true;
		}
		return false;
	}

	//响应网络请求
	virtual void time4msg()
	{
		auto t1 = _tTime.getElapsedSecond();
		if (t1 >= 1.0)
		{
			printf("thread<%d>,time<%lf>,socket<%d>,clients<%d>,recv<%d>,msg<%d>\n",
				(int)_cellServers.size(), t1, _sock, (int)_clientCount, (int)(_recvCount / t1), (int)(_msgCount / t1));
			_msgCount = 0;
			_recvCount = 0;
			_tTime.update();
		}
	}

	//1个线程调用
	void OnNetJoin(ClientSocket* pClient)
	{
		_clientCount++;
	}

	//4个线程调用
	void OnNetLeave(ClientSocket* pClient)
	{
		_clientCount--;
	}

	//4个线程调用
	void OnNetMsg(ClientSocket* pClient, DataHeader* header)
	{
		_msgCount++;
	}

	void OnNetRecv(ClientSocket* pClient)
	{
		_recvCount++;
	}

};
#endif
