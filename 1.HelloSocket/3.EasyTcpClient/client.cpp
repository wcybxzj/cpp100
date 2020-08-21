#include"EasyTcpClient.hpp"
//#include"CELLTimestamp.hpp"

//linux:make -f index.txt

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


void test1() {
	EasyTcpClient client;
	client.Connect("127.0.0.1", 4567);
	//client.Connect("65.49.211.61", 4567);
	//client.Connect("192.168.204.132", 4567);

	Login login;
	strcpy(login.userName, "ybx");
	strcpy(login.PassWord, "12345");

	while (client.isRun())
	{
		//printf("send login\n");
		client.SendData(&login);
		client.OnRun();
	}

	client.Close();

	printf("client已经退出\n");
	getchar();
}

void test2() {
	//windows:FD_SETSIZE 是64
	//linux:FD_SETSIZE 是1024

	//const int cCount = FD_SETSIZE-1;
	const int cCount = 1000;//windows: -1是服务监听的fd  所以客户端最多启动63个fd
	EasyTcpClient* client[cCount];

	for (int i = 0; i < cCount; i++)
	{
		client[i] = new EasyTcpClient;
	}

	for (int i = 0; i < cCount; i++)
	{
		//client[i]->Connect("127.0.0.1", 4567);
		//client[i]->Connect("65.49.211.61", 4567);
		client[i]->Connect("192.168.204.132", 4567);
	}

	std::thread t1(cmdThread);
	t1.detach();

	Login login;
	strcpy(login.userName, "ybx");
	strcpy(login.PassWord, "12345");
	
	while (g_bRun)
	{
		for (int i = 0; i < cCount; i++)
		{
			client[i]->SendData(&login);
			client[i]->OnRun();
		}
	}

	for (int i = 0; i < cCount; i++)
	{
		client[i]->Close();
	}

	printf("client已经退出\n");
	getchar();
}

int main() {
	//test1();
	test2();

	return 0;
}