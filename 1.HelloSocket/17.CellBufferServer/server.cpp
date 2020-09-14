//linux:make -f index.txt
#include"EasyTcpServer.hpp"

bool g_bRun = true;

class MyServer : public EasyTcpServer
{
public:

	//只会被一个线程触发 安全
	virtual void OnNetJoin(CellClient* pClient)
	{
		EasyTcpServer::OnNetJoin(pClient);
	}

	//cellServer 4 多个线程触发 不安全
	//如果只开启1个cellServer就是安全的
	virtual void OnNetLeave(CellClient* pClient)
	{
		EasyTcpServer::OnNetLeave(pClient);
	}

	virtual void OnNetMsg(CellServer *pCellServer, CellClient* pClient, netmsg_DataHeader* header)
	{
		EasyTcpServer::OnNetMsg(pCellServer, pClient, header);

		netmsg_Login* login;
		netmsg_Logout* logout;
		
		netmsg_LogoutR ret1;

		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			pClient->resetDTHeart();
			//继续将body部分读入到szRecv
			login = (netmsg_Login*)header;
			//printf("客户端:%d,收到命令:CMD_LOGIN, len:%d, username:%s, password:%s\n",
			//	cSock, login->dataLength, login->userName, login->PassWord);
			
			netmsg_LoginR ret;
			if (pClient->SendData(&ret) == SOCKET_ERROR)
			{
				CELLLog::Info("<Socket=%d> 用户定义发送缓冲区满了!\n", pClient->sockfd());
			}
			//netmsg_LoginR* ret = new netmsg_LoginR();
			//pCellServer->addSendTask(pClient, ret);
			break;
		}
		case CMD_LOGOUT:
		{
			logout = (netmsg_Logout*)header;
			//printf("客户端:%d,收到命令:CMD_LOGOUT, len:%d, username:%s\n",
			//	cSock, logout->dataLength, logout->userName);
			send(pClient->sockfd(), (const char*)&ret1, sizeof(netmsg_LogoutR), 0);

			//SendData(cSock, &ret1);
			break;
		}
		case CMD_C2S_HEART:
		{
			pClient->resetDTHeart();
			netmsg_c2s_Heart ret;
			pClient->SendData(&ret);
		}
		default:
		{
			CELLLog::Info("<socket=%d>收到未定义消息,数据长度：%d\n", pClient->sockfd(), header->dataLength);
			//DataHeader ret;
			//SendData(cSock, &ret);
			break;
		}
		}
	}
};


void test1()
{
	CELLLog::Instance().setLogPath("serverLog.txt", "w");

	MyServer server;
	server.InitSocket();
	server.Bind(nullptr, 4567);
	server.Listen(5);
	server.Start(4);

	char cmdBuf[256] = {};
	while (true)
	{
		scanf("%s", cmdBuf);
		if (0 == strcmp("exit", cmdBuf))
		{
			server.Close();
			CELLLog::Info("退出cmdThread线程!!!!\n");
			break;
		}
		else
		{
			CELLLog::Info("不支持的命令\n");
		}
	}

	CELLLog::Info("已退出。\n");
	//system("pause");

}

int main() {
	test1();
	return 0;
}