//linux:make -f index.txt
#ifndef _WIN32
	#include "Alloctor.hpp"
#endif // _WIN32
#include"EasyTcpServer.hpp"

bool g_bRun = true;

void cmdThread() {
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
		else 
		{
			printf("不支持的命令\n");
		}
	}
}

class MyServer : public EasyTcpServer
{
public:

	//只会被一个线程触发 安全
	virtual void OnNetJoin(CellClientPtr& pClient)
	{
		EasyTcpServer::OnNetJoin(pClient);
	}

	//cellServer 4 多个线程触发 不安全
	//如果只开启1个cellServer就是安全的
	virtual void OnNetLeave(CellClientPtr& pClient)
	{
		EasyTcpServer::OnNetLeave(pClient);
	}

	virtual void OnNetMsg(CellServer *pCellServer, CellClientPtr& pClient, netmsg_DataHeader* header)
	{
		EasyTcpServer::OnNetMsg(pCellServer, pClient, header);

		netmsg_Login* login;
		netmsg_Logout* logout;
		//netmsg_LoginR ret;


		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			//继续将body部分读入到szRecv
			login = (netmsg_Login*)header;
			//printf("客户端:%d,收到命令:CMD_LOGIN, len:%d, username:%s, password:%s\n",
			//	cSock, login->dataLength, login->userName, login->PassWord);

			//pClient->SendData(&ret);

			
			netmsg_LoginRPtr ret = std::make_shared<netmsg_LoginR>();
			netmsg_DataHeaderPtr ret1 = std::static_pointer_cast<netmsg_DataHeader>(ret);
			pCellServer->addSendTask(pClient, ret1);
			break;
		}
		case CMD_LOGOUT:
		{
			netmsg_LogoutR ret1;
			logout = (netmsg_Logout*)header;
			//printf("客户端:%d,收到命令:CMD_LOGOUT, len:%d, username:%s\n",
			//	cSock, logout->dataLength, logout->userName);
			send(pClient->sockfd(), (const char*)&ret1, sizeof(netmsg_LogoutR), 0);

			//SendData(cSock, &ret1);
			break;
		}
		default:
		{
			printf("<socket=%d>收到未定义消息,数据长度：%d\n", pClient->sockfd(), header->dataLength);
			//DataHeader ret;
			//SendData(cSock, &ret);
			break;
		}
		}
	}

private:

};


int main() {
	MyServer server;
	server.InitSocket();
	server.Bind(nullptr, 4567);
	server.Listen(5);
	server.Start(4);

	//启动UI线程
	std::thread t1(cmdThread);
	t1.detach();

	while (g_bRun)
	{
		server.OnRun();
		//printf("空闲时间处理其它业务..\n");
	}
	server.Close();
	printf("已退出。\n");
	getchar();

	printf("服务器已经退出\n");
	getchar();
	return 0;
}