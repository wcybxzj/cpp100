#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#include"CELL.hpp"
#include"CELLClient.hpp"
#include"CELLServer.hpp"
#include"INetEvent.hpp"

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
			addClientToCellServer(new CellClient(cSock));
			//inet_ntoa(clientAddr.sin_addr);// client ip
		}
		return cSock;
	}

	void addClientToCellServer(CellClient* pClient) {
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
	}

	void Start(int nCellServer) {
		for (int  i= 0; i < nCellServer; i++)
		{
			auto ser = new CellServer(i+1);
			_cellServers.push_back(ser);
			ser->setEventObj(this);
			ser->Start();
		}
	}

	//关闭socket
	void Close() {
		printf("EasyTcpServer close() begin()\n");
		if (_sock != INVALID_SOCKET)
		{
			for (auto s : _cellServers)
			{
				delete s;
			}

#ifdef _WIN32
			closesocket(_sock);
			//清除Windows socket环境
			WSACleanup();
#else
			// 8 关闭套节字closesocket
			close(_sock);
#endif
		}
		_sock = INVALID_SOCKET;
		printf("EasyTcpServer close() end()\n");
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
				printf("EasyTcpServer() select < 0  error!!!\n");
				return false;
			}
			else if(select_ret == 0) 
			{
				//printf("EasyTcpServer() select == 0!!!\n");
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
	void OnNetJoin(CellClient* pClient)
	{
		_clientCount++;
	}

	//4个线程调用
	void OnNetLeave(CellClient* pClient)
	{
		_clientCount--;
	}

	//4个线程调用
	void OnNetMsg(CellServer*pCellServer, CellClient* pClient, netmsg_DataHeader* header)
	{
		_msgCount++;
	}

	void OnNetRecv(CellClient* pClient)
	{
		_recvCount++;
	}

};
#endif
