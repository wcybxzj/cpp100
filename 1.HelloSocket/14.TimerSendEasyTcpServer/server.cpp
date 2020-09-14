//linux:make -f index.txt
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
			pClient->SendData(&ret);

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
			printf("<socket=%d>收到未定义消息,数据长度：%d\n", pClient->sockfd(), header->dataLength);
			//DataHeader ret;
			//SendData(cSock, &ret);
			break;
		}
		}
	}

private:

};


void test1()
{
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif

#ifndef WIN32
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask, SIGPIPE);
	int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
	if (rc != 0) {
		printf("block sigpipe error\n");
	}
#endif 

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
}

//如果不使用CellSemaphore来保证task.Close()时候task线程已经退出就会造成问题
//有问题时候的输出:
//CellTaskServer - 1.Close begin
//CellTaskServer - 1.Close end
//CellTaskServer - 1.OnRun exit--->退出了还执行

//正确的输出:
//CellTaskServer - 1.Close begin
//CellTaskServer - 1.OnRun exit-->OK
//CellTaskServer - 1.Close end
void test2(){
	CellTaskServer task;
	task.Start();
	Sleep(1);
	task.Close();
	system("pause");
}

int main() {
	test1();
	//test2();

	return 0;
}