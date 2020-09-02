#ifndef _MessageHeader_hpp_
#define _MessageHeader_hpp_

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct netmsg_DataHeader
{
	netmsg_DataHeader()
	{
		dataLength = sizeof(netmsg_DataHeader);
		cmd = CMD_ERROR;
	}
	short dataLength;
	short cmd;
};

//DataPackage
struct netmsg_Login : public netmsg_DataHeader
{
	netmsg_Login()
	{
		dataLength = sizeof(netmsg_Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
	char data[32];
};

struct netmsg_LoginR : public netmsg_DataHeader
{
	netmsg_LoginR()
	{
		dataLength = sizeof(netmsg_LoginR);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
	char data[92];
};


struct netmsg_Logout : public netmsg_DataHeader
{
	netmsg_Logout()
	{
		dataLength = sizeof(netmsg_Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct netmsg_LogoutR : public netmsg_DataHeader
{
	netmsg_LogoutR()
	{
		dataLength = sizeof(netmsg_LogoutR);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct netmsg_NewUserJoin : public netmsg_DataHeader
{
	netmsg_NewUserJoin()
	{
		dataLength = sizeof(netmsg_NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		scok = 0;
	}
	int scok;
};




//int a1 = sizeof(netmsg_DataHeader);
//int a2 = sizeof(netmsg_Login);
//int a3 = sizeof(netmsg_LoginR);
//int a4 = sizeof(netmsg_Logout);
//int a5 = sizeof(netmsg_LogoutR);
//int a6 = sizeof(netmsg_NewUserJoin);

#endif // !_MessageHeader_hpp_