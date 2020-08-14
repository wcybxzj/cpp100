#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
//inet_ntoa����

//�������1:
#define _WINSOCK_DEPRECATED_NO_WARNINGS

//�������2:
//����c++  -> Ԥ���� -> Ԥ����������

//�������3:
//c/c++  -> ���� -> sdl���ѡ���



//�����������ĺ� ����˵winsock2.h�����ظ�����ĳ���
#include<Windows.h>
#include<WinSock2.h>
#include<stdio.h>

//����1:����Windows ��̬��
#pragma comment(lib, "ws2_32.lib")


//����2:����->��������->������->����->����������->д��ws2_32.lib

enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
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

int main() {
	//����windows socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat; 
	WSAStartup(ver, &dat);

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
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
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

	//accept
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;

	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock){
		printf("accept errror!!!");
	}
	else {
		printf("accept new client fd:%d ,ip:%s\n", _cSock, inet_ntoa(clientAddr.sin_addr));
	}

	while (true)
	{
		DataHeader header = {};
		//recv
		int nLen = recv(_cSock, (char *) &header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("client closed!");
			closesocket(_cSock);
			break;
		}

		Login login = {};
		Logout logout = {};
		LoginResult ret;
		LogoutResult ret1;

		switch (header.cmd)
		{
			case CMD_LOGIN: 
				recv(_cSock, (char*)&login+sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
				printf("�յ�����:CMD_LOGIN, len:%d, username:%s, password:%s\n",
							login.dataLength, login.userName, login.passWord);

				send(_cSock, (const char*)&ret, sizeof(LoginResult), 0);

				break;

			case CMD_LOGOUT:
				recv(_cSock, (char*)&logout+sizeof(DataHeader), sizeof(logout)-sizeof(DataHeader), 0);
				printf("�յ�����:CMD_LOGOUT, len:%d, username:%s\n",
					logout.dataLength, logout.userName);

				send(_cSock, (const char*)&ret1, sizeof(LogoutResult), 0);
				break;

			default:
				header.cmd = CMD_ERROR;
				header.dataLength = 0;
				send(_cSock, (const char*)&header, sizeof(DataHeader), 0);
				break;
		}
	}

	//close
	closesocket(_sock);

	//����windows socket
	WSACleanup();
	printf("�������Ѿ��˳�\n");
	getchar();
	return 0;;
}