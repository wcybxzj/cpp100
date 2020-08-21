#include"EasyTcpClient.hpp"
//#include"CELLTimestamp.hpp"

//linux:make -f index.txt
void cmdThread(EasyTcpClient* client) {
	char cmdBuf[256] = {};
	while (true)
	{
		scanf("%s", cmdBuf);
		if (0 == strcmp("exit", cmdBuf))
		{
			client->Close();
			printf("退出cmdThread线程!!!!\n");
			break;
		}
		else if (0 == strcmp("login", cmdBuf))
		{
			Login login;
			strcpy(login.userName, "ybx");
			strcpy(login.PassWord, "12345");
			client->SendData(&login);
		}
		else if (0 == strcmp("logout", cmdBuf))
		{
			Logout logout;
			strcpy(logout.userName, "ybx");
			client->SendData(&logout);
		}
	}
}


//手动输入版
void test1() {
	EasyTcpClient client;
	client.Connect("127.0.0.1", 4567);
	//client.Connect("65.49.211.61", 4567);
	//client.Connect("192.168.204.132", 4567);

	std::thread t1(cmdThread, &client);
	t1.detach();

	while (client.isRun())
	{
		client.OnRun();
	}

	client.Close();

	printf("client已经退出\n");
	getchar();
}

//自动发送版
void test2() {
	EasyTcpClient client;
	client.Connect("127.0.0.1", 4567);
	//client.Connect("65.49.211.61", 4567);
	//client.Connect("192.168.204.132", 4567);

	Login login;
	strcpy(login.userName, "ybx");
	strcpy(login.PassWord, "12345");

	while (client.isRun())
	{
		client.OnRun();
		client.SendData(&login);
	}

	client.Close();

	printf("client已经退出\n");
	getchar();
}

int main() {
	//test1();
	test2();
	return 0;
}