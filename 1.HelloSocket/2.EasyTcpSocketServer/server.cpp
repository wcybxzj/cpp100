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

//linux:make -f index.txt

enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHeader
{
	short dataLength;
	short cmd;
};

struct Login :public DataHeader {
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
		memset(userName,0, sizeof(userName));
		memset(passWord,0, sizeof(passWord));
	}

	char userName[32];
	char passWord[32];
};

struct LoginResult :public DataHeader {
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin :public DataHeader {
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};



std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock) {
	char szRecv[4096] = {};

	//只读取header到szRecv
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("client closed!fd:%d\n",_cSock);
		return -1;
	}

	Login* login;
	Logout* logout;
	LoginResult ret;
	LogoutResult ret1;

	switch (header->cmd)
	{
		case CMD_LOGIN:
			//继续将body部分读入到szRecv
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			login = (Login*)szRecv;
			printf("客户端:%d,收到命令:CMD_LOGIN, len:%d, username:%s, password:%s\n",
				_cSock,login->dataLength, login->userName, login->passWord);
			send(_cSock, (const char*)&ret, sizeof(LoginResult), 0);
			break;

		case CMD_LOGOUT:
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			logout = (Logout*)szRecv;
			printf("客户端:%d,收到命令:CMD_LOGOUT, len:%d, username:%s\n",
				_cSock, logout->dataLength, logout->userName);

			send(_cSock, (const char*)&ret1, sizeof(LogoutResult), 0);
			break;

		default:
			DataHeader header = { 0, CMD_ERROR };
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			break;
	}
}


int main() {
#ifdef _WIN32
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//------------
#endif
	//socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == _sock)
	{
		printf("server: socket create error!!!!!!\n");
	}
	else {
		printf("server: socket create ok!!!!!!\n");
	}
	
	//bind
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
#else
	_sin.sin_addr.s_addr = INADDR_ANY;
#endif
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("bind errro!!!!!\n");
	}
	else {
		printf("bind ok!!!!!\n");
	}
	
	//listen
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("listen errro!!!!!\n");
	}
	else {
		printf("listen ok!!!!!\n");
	}
	SOCKET _cSock = INVALID_SOCKET;

	while (true)
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
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
			if (maxSock < g_clients[n])
			{
				maxSock = g_clients[n];
			}
		}


		timeval t = { 1,0 };
		int select_ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);//非阻塞select
		//int select_ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, NULL);//阻塞select
		if (select_ret <0)
		{
			printf("select < 0  error!!!\n");
			break;
		}

		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);

			//accept
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
#ifdef _WIN32
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
			_cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif

			if (INVALID_SOCKET == _cSock) {
				printf("accept errror!!!\n");
			}
			else {

				for (int n = (int)g_clients.size() - 1; n >= 0; n--)
				{
					NewUserJoin userJoin;
					send(g_clients[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
				}
				g_clients.push_back(_cSock);
				printf("新客户端加入：socket = %d,IP = %s \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
			}
		}

		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			if (FD_ISSET(g_clients[n], &fdRead))
			{
				if (-1 == processor(g_clients[n]))
				{
					auto iter = g_clients.begin() + n;//std::vector<SOCKET>::iterator
					if (iter != g_clients.end())
					{
						g_clients.erase(iter);
					}
				}
			}
		}

		printf("空闲时间处理其它业务..\n");
	}

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

	printf("服务器已经退出\n");
	getchar();
	return 0;;
}