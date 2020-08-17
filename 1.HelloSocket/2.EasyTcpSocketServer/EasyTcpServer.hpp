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
class EasyTcpServer
{

private:
	SOCKET _sock;
	std::vector<SOCKET> g_clients;

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

			g_clients.push_back(cSock);
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
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				closesocket(g_clients[n]);
			}
			// 8 关闭套节字closesocket
			closesocket(_sock);
			//------------
			//清除Windows socket环境
			WSACleanup();
#else
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				close(g_clients[n]);
			}
			// 8 关闭套节字closesocket
			close(_sock);
#endif
		}
	}

	//处理网络请求
	bool OnRun() {
		SOCKET _cSock;
		//if (isRun())
		//{
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
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(g_clients[n], &fdRead);
				if (maxSock < g_clients[n])
				{
					maxSock = g_clients[n];
				}
			}

			timeval t = { 0,0 };//non-block 浪费cpu
			int select_ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);//非阻塞select
			//int select_ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, NULL);//阻塞select
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

			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(g_clients[n], &fdRead))
				{
					if (-1 == RecvData(g_clients[n]))
					{
						auto iter = g_clients.begin() + n;//std::vector<SOCKET>::iterator
						if (iter != g_clients.end())
						{
							g_clients.erase(iter);
						}
					}
				}
			}

			//printf("空闲时间处理其它业务..\n");
			return true;
		//}
		//return false;
	}

	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
		//return _sock != INVALID_SOCKET && _isConnect;

	}

	//接受数据 处理粘包 拆分包
	int RecvData(SOCKET _cSock) {
		char szRecv[4096] = {};

		//只读取header到szRecv
		int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if (nLen <= 0)
		{
			printf("client closed!fd:%d\n", _cSock);
			return -1;
		}

		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		
		OnNetMsg(_cSock, header);


		return 0;
	}

	//响应网络请求
	virtual void OnNetMsg(SOCKET _cSock, DataHeader* header)
	{
		Login* login;
		Logout* logout;
		LoginResult ret;
		LogoutResult ret1;

		static int num = 0;
		printf("OnNetMsg num:%d\n", num);
		switch (header->cmd)
		{
		case CMD_LOGIN:
			//继续将body部分读入到szRecv
			login = (Login*)header;
			printf("客户端:%d,收到命令:CMD_LOGIN, len:%d, username:%s, password:%s\n",
				_cSock, login->dataLength, login->userName, login->PassWord);
			send(_cSock, (const char*)&ret, sizeof(LoginResult), 0);
			break;

		case CMD_LOGOUT:
			logout = (Logout*)header;
			printf("客户端:%d,收到命令:CMD_LOGOUT, len:%d, username:%s\n",
				_cSock, logout->dataLength, logout->userName);

			send(_cSock, (const char*)&ret1, sizeof(LogoutResult), 0);
			break;

		default:
			header->cmd = 0;
			header->dataLength = CMD_ERROR;
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			break;
		}
		num++;
	}

	//发送指定socket数据
	int SendData(SOCKET _cSock, DataHeader* header)
	{
		int ret = SOCKET_ERROR;
		if (isRun() && header)
		{
			ret = send(_cSock, (const char*)header, header->dataLength, 0);
			if (SOCKET_ERROR == ret)
			{
				Close();
			}
		}
		return ret;
	}

	//发送指定socket数据
	void SendDataToAll(DataHeader* header)
	{
		int ret = SOCKET_ERROR;
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			SendData(g_clients[n], header);
		}
	}

};
#endif
