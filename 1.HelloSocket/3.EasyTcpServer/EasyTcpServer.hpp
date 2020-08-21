//#define _CRT_SECURE_NO_WARNINGS
//#define WIN32_LEAN_AND_MEAN
////inet_ntoa报错
////解决方法1:
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
////解决方法2:
////属性c++  -> 预处器 -> 预处理器定义
////解决方法3:
////c/c++  -> 常规 -> sdl检查选择否
////如果不加上面的宏 报错说winsock2.h中有重复定义的常量
//#include <iostream>
//#include<Windows.h>
//#include<WinSock2.h>
//#include<stdio.h>
//#include<vector>
//#include <ws2tcpip.h>
//#include <thread>
////方法1:加载Windows 静态库
//#pragma comment(lib, "ws2_32.lib")
//using namespace std;
////方法2:属性->配置属性->连接器->输入->附加依赖项->写入ws2_32.lib

#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_
#ifdef _WIN32
	#define FD_SETSIZE 1024 //windows select支持超过64个fd 定义在WinSock2.h前面
	#define _CRT_SECURE_NO_WARNINGS
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
	#include<Windows.h>
	#include<WinSock2.h>
	#include<stdio.h>
	#include<vector>
	#include <ws2tcpip.h>
	#pragma comment(lib,"ws2_32.lib")
#else
	#include<unistd.h> //uni std
	#include<arpa/inet.h>
	#include<string.h>
	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif

#include <iostream>
#include <thread>
#include<stdio.h>
#include<vector>

#include "MessageHeader.hpp"

#define  _EasyTcpServer_hpp_

#ifndef RECV_BUFF_SIZE
	//缓冲区
	#define RECV_BUFF_SIZE 10240 //10KB
#endif // !RECV_BUFF_SIZE

class ClientSocket
{
private:
	//消息缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE * 10];//100KB
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
};

class EasyTcpServer
{
private:
	SOCKET _sock;
	std::vector<ClientSocket*> _clients;//尽量用堆内存 c++栈内存太小

public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
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

		if (INVALID_SOCKET == cSock) {
			printf("socket=<%d>,错误, 接收到无效SOCKET... \n",(int)_sock);
		}
		else {
			NewUserJoin userJoin;
			SendDataToAll(&userJoin);

			_clients.push_back(new ClientSocket(cSock));
			printf("socket=<%d>,新客户端加入：socket = %d,IP = %s \n",
				(int)cSock, (int)_sock, inet_ntoa(clientAddr.sin_addr));
		}
		return cSock;
	}

	//关闭socket
	void Close() {
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				//closesocket(_clients[n]->sockfd());
				delete _clients[n];
			}
			// 8 关闭套节字closesocket
			closesocket(_sock);
			//------------
			//清除Windows socket环境
			WSACleanup();
#else
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				close(_clients[n]->sockfd());
				delete _clients[n];
			}
			// 8 关闭套节字closesocket
			close(_sock);
#endif
			_clients.clear();
		}
	}

	int select_count = 0;
	//处理网络请求
	bool OnRun() {
		SOCKET _cSock;
		if (isRun())
		{
			fd_set fdRead;
			fd_set fdWrite;
			fd_set fdExp;

			FD_ZERO(&fdRead);
			FD_ZERO(&fdWrite);
			FD_ZERO(&fdExp);

			FD_SET(_sock, &fdRead);
			FD_SET(_sock, &fdWrite);
			FD_SET(_sock, &fdExp);

			SOCKET maxSock = _sock;
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(_clients[n]->sockfd(), &fdRead);
				if (maxSock < _clients[n]->sockfd())
				{
					maxSock = _clients[n]->sockfd();
				}
			}

			select_count++;
			timeval t = { 0,0 };
			
			int select_ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);//非阻塞select
			//int select_ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, NULL);//阻塞
			//printf("select_count:%d, select_ret:%d\n", select_count, select_ret);
			
			if (select_ret < 0)
			{
				Close();
				printf("select < 0  error!!!\n");
				return false;
			}
			static int num=0;
			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				_cSock = Accept();
				if (INVALID_SOCKET == _cSock) {
					printf("accept errror!!!\n");
				}
				else {
					num++;
					printf("accept number:%d\n", num);
				}
			}

			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(_clients[n]->sockfd(), &fdRead))
				{
					if (-1 == RecvData(_clients[n]))
					{
						auto iter = _clients.begin() + n;//std::vector<SOCKET>::iterator
						if (iter != _clients.end())
						{
							delete _clients[n];
							_clients.erase(iter);
						}
					}
				}
			}

			//printf("空闲时间处理其它业务..\n");
			return true;
		}
		return false;
	}

	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
		//return _sock != INVALID_SOCKET && _isConnect;
	}

	char _szRecv[RECV_BUFF_SIZE] = {};
	//接受数据 处理粘包 拆分包
	int RecvData(ClientSocket* pClient) {
		//只读取header到szRecv
		int nLen = recv(pClient->sockfd(), _szRecv, RECV_BUFF_SIZE, 0);

		if (nLen <= 0)
		{
			if (nLen == 0)
			{
				printf("recv nLen:%d\n", nLen);
			}
			else {
				printf("recv nLen:%d\n", nLen);
			}
			printf("client closed!fd:%d\n", pClient->sockfd());
			return -1;
		}

		memcpy(pClient->msgBuf()+pClient->getLastPos(), _szRecv, nLen);
		pClient->setLastPos(pClient->getLastPos()+nLen);

		while (pClient->getLastPos() >= sizeof(DataHeader))
		{
			DataHeader* header = (DataHeader*)pClient->msgBuf();
			if (pClient->getLastPos() >= header->dataLength)
			{
				int nSize = pClient->getLastPos() - header->dataLength;
				OnNetMsg(pClient->sockfd(), header);
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
	virtual void OnNetMsg(SOCKET cSock, DataHeader* header)
	{
		Login* login;
		Logout* logout;
		LoginResult ret;
		LogoutResult ret1;

		static int num = 0;
		//printf("OnNetMsg num:%d\n", num);
		
		switch (header->cmd)
		{
		case CMD_LOGIN:
			//继续将body部分读入到szRecv
			login = (Login*)header;
			//printf("客户端:%d,收到命令:CMD_LOGIN, len:%d, username:%s, password:%s\n",
			//	cSock, login->dataLength, login->userName, login->PassWord);
			SendData(cSock, &ret);
			break;

		case CMD_LOGOUT:
			logout = (Logout*)header;
			//printf("客户端:%d,收到命令:CMD_LOGOUT, len:%d, username:%s\n",
			//	cSock, logout->dataLength, logout->userName);
			send(cSock, (const char*)&ret1, sizeof(LogoutResult), 0);
			SendData(cSock, &ret1);
			break;
		default:
			printf("<socket=%d>收到未定义消息,数据长度：%d\n", _sock, header->dataLength);
			//DataHeader ret;
			//SendData(cSock, &ret);
			break;
		}
		num++;
	}

	//发送指定socket数据
	int SendData(SOCKET cSock, DataHeader* header)
	{
		int ret = SOCKET_ERROR;
		if (isRun() && header)
		{
			ret = send(cSock, (const char*)header, header->dataLength, 0);
			if (ret == SOCKET_ERROR)
			{
				printf("send error!\n");
			}
		}
		return ret;
	}

	//发送指定socket数据
	void SendDataToAll(DataHeader* header)
	{
		int ret = SOCKET_ERROR;
		for (int n = (int)_clients.size() - 1; n >= 0; n--)
		{
			SendData(_clients[n]->sockfd(), header);
		}
	}
};
#endif
