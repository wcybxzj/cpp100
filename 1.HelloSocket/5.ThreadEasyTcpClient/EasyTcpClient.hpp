/*
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

//inet_ntoa报错
//解决方法1:
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//解决方法2:
//属性c++  -> 预处器 -> 预处理器定义

//如果不加上面的宏 报错说winsock2.h中有重复定义的常量
#include <iostream>
#include<Windows.h>
#include<WinSock2.h>
#include<stdio.h>
#include<vector>
#include <ws2tcpip.h>
#include <thread>
//方法1:加载Windows 静态库
#pragma comment(lib, "ws2_32.lib")
//方法2:属性->配置属性->连接器->输入->附加依赖项->写入ws2_32.lib
*/

#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_

#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
	#define FD_SETSIZE 10024 //windows select支持超过64个fd 定义在WinSock2.h前面
	#include<windows.h>
	#include<WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")
#else
	#include<unistd.h> //uni std
	#include<arpa/inet.h>
	#include<string.h>
	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif

#include<stdio.h>
#include<vector>
#include<map>
#include<thread>
#include<mutex>
#include<atomic>
#include<functional>
#include"MessageHeader.hpp"
#include"CELLTimestamp.hpp"

class EasyTcpClient
{
	SOCKET _sock;
	bool _isConnect;
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
		_isConnect = false;
	}

	virtual ~EasyTcpClient()
	{
		Close();
	}

	//初始化socket
	void InitSocket()
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
	}

	//连接服务器
	int Connect(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		// 2 连接服务器 connect
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		//printf("<socket=%d>正在连接服务器<%s:%d>...\n", _sock, ip, port);
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("<socket=%d>错误，连接服务器<%s:%d>失败...\n", _sock, ip, port);
		}
		else {
			_isConnect = true;
			printf("<socket=%d>连接服务器<%s:%d>成功...\n",_sock, ip, port);
		}
		return ret;
	}

	//关闭套节字closesocket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(_sock);
			//清除Windows socket环境
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}
		_isConnect = false;
	}

	int select_count = 0;

	//处理网络消息
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);
			timeval t = { 1,0 };
			select_count++;
			int select_ret = select(_sock + 1, &fdReads, 0, 0, NULL);
			//int select_ret = select(_sock + 1, &fdReads, 0, 0, &t);
			//printf("select_count:%d, select_ret:%d\n", select_count, select_ret);

			if (select_ret < 0)
			{
				printf("<socket=%d>select任务结束1\n", _sock);
				Close();
				return false;
			}
			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);

				if (-1 == RecvData(_sock))
				{
					printf("<socket=%d>select任务结束2\n", _sock);
					Close();
					return false;
				}
			}
			return true;
		}
		return false;
	}

	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET && _isConnect;
	}

#ifndef RECV_BUFF_SIZE
	//缓冲区
	#define RECV_BUFF_SIZE 10240 //10KB
#endif // !RECV_BUFF_SIZE

	char _szMsgBuf[RECV_BUFF_SIZE] = {};

	int _lastPos = 0;
	//接收数据 处理粘包 拆分包
	int RecvData(SOCKET cSock)
	{
		// 5 接收数据
		char* szRecv = _szMsgBuf + _lastPos;
		int nLen = (int)recv(cSock, szRecv, (RECV_BUFF_SIZE)-_lastPos, 0);
		//printf("nLen=%d\n", nLen);
		if (nLen <= 0)
		{
			printf("<socket=%d>与服务器断开连接，任务结束。\n", cSock);
			return -1;
		}
		_lastPos += nLen;
		while (_lastPos >= sizeof(netmsg_DataHeader))
		{
			netmsg_DataHeader* header = (netmsg_DataHeader*)_szMsgBuf;
			if (_lastPos >= header->dataLength)
			{
				int nSize = _lastPos - header->dataLength;
				OnNetMsg(header);
				memcpy(_szMsgBuf, _szMsgBuf + header->dataLength, nSize);
				_lastPos = nSize;
			}
			else {
				//消息缓冲区剩余数据不够一条完整消息
				break;
			}
		}
		return 0;
	}


	//响应网络消息
	virtual void OnNetMsg(netmsg_DataHeader* header)
	{
		static int num = 0;
		//printf("OnNetMsg num:%d\n", num);

		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{

			netmsg_LoginR* login = (netmsg_LoginR*)header;
			//printf("<socket=%d>收到服务端消息：CMD_LOGIN_RESULT,数据长度：%d\n", _sock, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			netmsg_LogoutR* logout = (netmsg_LogoutR*)header;
			//printf("<socket=%d>收到服务端消息：CMD_LOGOUT_RESULT,数据长度：%d\n", _sock, logout->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			netmsg_NewUserJoin* userJoin = (netmsg_NewUserJoin*)header;
			//printf("<socket=%d>收到服务端消息：CMD_NEW_USER_JOIN,数据长度：%d\n", _sock, userJoin->dataLength);
		}
		break;
		case CMD_ERROR:
		{
			printf("<socket=%d>收到服务端消息：CMD_ERROR,数据长度：%d\n", _sock, header->dataLength);
		}
		break;
		default:
		{
			printf("<socket=%d>收到未定义消息,数据长度：%d\n", _sock, header->dataLength);
		}
		}
		num++;
	}

	//发送数据
	int SendData(netmsg_DataHeader* header, int nLen)
	{
		int ret = SOCKET_ERROR;
		if (isRun() && header)
		{
			ret = send(_sock, (const char*)header, header->dataLength, 0);
			if (SOCKET_ERROR == ret)
			{
				printf("send errror， 与服务器断开连接，任务结束。\n");
				Close();
			}
		}
		return ret;
	}


private:

};

#endif