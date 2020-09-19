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

#include"CELL.hpp"
#include "CELLClient.hpp"

class EasyTcpClient
{

protected:
	CellClient* _pClient = nullptr;
	bool _isConnect;

public:
	EasyTcpClient()
	{
		_isConnect = false;
	}

	virtual ~EasyTcpClient()
	{
		Close();
	}

	//初始化socket
	void InitSocket()
	{
		CELLNetWork::Init();

		if (_pClient)
		{
			CELLLog::Info("warning, initSocket error!\n",
				(int)_pClient->sockfd());
			Close();
		}
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == sock)
		{
			CELLLog::Info("错误，建立Socket失败...\n");
		}
		else {
			//CELLLog::Info("建立Socket=<%d>成功...\n", _sock);
			_pClient = new CellClient(sock);
		}
	}

	//连接服务器
	int Connect(const char* ip, unsigned short port)
	{
		if (!_pClient)
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
		int ret = connect(_pClient->sockfd(), (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			CELLLog::Info("<socket=%d>错误，连接服务器<%s:%d>失败...\n",
				_pClient->sockfd(), ip, port);
		}
		else {
			_isConnect = true;
			CELLLog::Info("<socket=%d>连接服务器<%s:%d>成功...\n", 
				_pClient->sockfd(), ip, port);
		}
		return ret;
	}

	//关闭套节字closesocket
	void Close()
	{
		if (_pClient)
		{
			delete _pClient;
			_pClient = nullptr;
		}
		_isConnect = false;
	}

	//处理网络消息
	bool OnRun()
	{
		if (isRun())
		{
			SOCKET _sock = _pClient->sockfd();

			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);

			fd_set fdWrite;
			FD_ZERO(&fdWrite);

			timeval t = { 0, 1 };
			int select_ret = 0;

			//说明这个_pClient的用户定义写缓冲有数据可以发
			if (_pClient->needWrite())
			{
				FD_SET(_sock, &fdWrite);
				select_ret = select(_sock + 1, &fdReads, &fdWrite, 0, &t);
			}
			else 
			{
				select_ret = select(_sock + 1, &fdReads,0 ,0 , &t);
			}

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
					CELLLog::Info("error, <socket=%d> EasyTcpClient OnRun()->RecvData() error\n", 
						_sock);
					Close();
					return false;
				}
			}


			if (FD_ISSET(_sock, &fdWrite))
			{
				FD_CLR(_sock, &fdWrite);
				if (-1 == _pClient->SendDataReal())
				{
					CELLLog::Info("error, <socket=%d> EasyTcpClient OnRun()->SendDataReal() error\n",
						_sock);
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
		return _pClient && _isConnect;
	}

	
	//接收数据 处理粘包 拆分包
	int RecvData(SOCKET cSock)
	{
		if (isRun())
		{
			int nLen = _pClient->RecvData();
			if (nLen > 0)
			{
				while (_pClient->hasMsg()) {
					OnNetMsg(_pClient->front_msg());
					_pClient->pop_front_msg();
				}
			}
			return nLen;
		}
		return 0;
	}


	//响应网络消息
	virtual void OnNetMsg(netmsg_DataHeader* header) = 0;
	
	//发送数据
	int SendData(netmsg_DataHeader* header)
	{
		if (isRun())
		{
			return _pClient->SendData(header);
		}
		return 0;
	}

	int SendData(const char* pData, int len)
	{
		if (isRun())
		{
			return _pClient->SendData(pData, len);
		}
		return 0;
	}

private:

};

#endif