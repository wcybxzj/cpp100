#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
//inet_ntoa����

//�������1:
#define _WINSOCK_DEPRECATED_NO_WARNINGS

//�������2:
//����c++  -> Ԥ���� -> Ԥ����������

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


int main() {
	//����windows socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

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
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ret = connect(_sock, (sockaddr*)&_sin, sizeof(_sin));
	if (SOCKET_ERROR == ret)
	{
		printf("connect errro!!!!!\n");
	}
	else {
		printf("connect ok!!!!!\n");
	}

	//recv
	while (true)
	{	
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("�յ�exit ����\n");
			break;
		}else if (0 == strcmp(cmdBuf, "login")){
			//send header and body
			Login login;
			strcpy(login.userName, "ybx");
			strcpy(login.passWord, "12345");
			send(_sock, (const char*)&login, sizeof(login), 0);

			//recv
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);

			printf("LoginResult:%d", loginRet.result);
		}else if (0 == strcmp(cmdBuf, "logout")) {
			//send header and body
			Logout logout;
			strcpy(logout.userName, "ybx");
			send(_sock, (const char*)&logout, sizeof(logout), 0);

			//recv
			LogoutResult logoutResult = {};
			recv(_sock, (char*)&logoutResult, sizeof(logoutResult), 0);
			printf("logoutResult:%d", logoutResult.result);
		}
		else {
			printf("unknown command!");
		}
	}


	//close
	closesocket(_sock);

	//����windows socket
	WSACleanup();

	printf("client�Ѿ��˳�\n");
	getchar();
	
	return 0;
}