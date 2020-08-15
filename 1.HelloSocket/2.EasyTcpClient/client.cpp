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
		memset(userName, 0, sizeof(userName));
		memset(passWord, 0, sizeof(passWord));
	}

	char userName[32];
	char passWord[32];
};

struct LoginResult :public DataHeader {
	LoginResult() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Logout :public DataHeader {
	Logout() {
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
		memset(userName, 0, sizeof(userName));
	}
	char userName[32];
};

struct LogoutResult :public DataHeader {
	LogoutResult() {
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


int processor(SOCKET _cSock) {
	char szRecv[1024] = {};

	//只读取header到szRecv
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("和服务器断开链接!!\n");
		return -1;
	}

	Login* login;
	Logout* logout;
	LoginResult *loginResult;
	LogoutResult *logoutResult;
	NewUserJoin* newUserJoin;

	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginResult* login = (LoginResult*)szRecv;
		printf("收到服务端消息：CMD_LOGIN_RESULT,数据长度：%d\n", login->dataLength);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogoutResult* logout = (LogoutResult*)szRecv;
		printf("收到服务端消息：CMD_LOGOUT_RESULT,数据长度：%d\n", logout->dataLength);
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		NewUserJoin* userJoin = (NewUserJoin*)szRecv;
		printf("收到服务端消息：CMD_NEW_USER_JOIN,数据长度：%d\n", userJoin->dataLength);
	}
	break;
	}
	return 0;
}


bool g_bRun = true;

void cmdThread(SOCKET _sock) {
	char cmdBuf[256] = {};
	while (true)
	{
		scanf("%s", cmdBuf);
		if (0 == strcmp("exit", cmdBuf))
		{
			g_bRun = false;
			printf("退出cmdThread线程!!!!\n");
			break;
		}
		else if (0 == strcmp("login", cmdBuf))
		{
			Login login;
			strcpy(login.userName, "ybx");
			strcpy(login.passWord, "12345");
			send(_sock, (const char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp("logout", cmdBuf))
		{
			Logout logout;
			strcpy(logout.userName, "ybx");
			send(_sock, (const char*)&logout, sizeof(logout), 0);
		}
	}
}

int main() {

#ifdef _WIN32
	//启动windows socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif

	//socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("socket create error!!!!!!\n");
	}
	else {
		printf("socket create ok!!!!!!\n");
	}

	//connect
	int ret;
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);

#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.204.132");
#else
	_sin.sin_addr.s_addr = inet_addr("0.0.0.0");
#endif


	ret = connect(_sock, (sockaddr*)&_sin, sizeof(_sin));
	if (SOCKET_ERROR == ret)
	{
		printf("connect errro!!!!!\n");
	}
	else {
		printf("connect ok!!!!!\n");
	}

	std::thread t1(cmdThread, _sock);
	t1.detach();

	//recv
	while (g_bRun)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);
		timeval t = { 1,0 };
		int select_ret = select(_sock + 1, &fdRead, 0 , 0, &t);//非阻塞select

		if (select_ret < 0)
		{
			printf("select error!");
			break;
		}

		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);

			if (-1 == processor(_sock))
			{
				printf("select 任务结束\n");
				break;
			}
		}
	}


#ifdef _WIN32
	closesocket(_sock);
	//清除Windows socket环境
	WSACleanup();
#else
	close(_sock);
#endif


	printf("client已经退出\n");
	getchar();
	
	return 0;
}