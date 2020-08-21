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

void  test1() {
	EasyTcpServer server;
	server.Bind(nullptr, 4567);
	server.Listen(5);


	while (server.isRun())
	{
		server.OnRun();

	}
	server.Close();
}

void test2() {
	EasyTcpServer server;
	server.Bind(nullptr, 4567);
	server.Listen(5);

	std::thread t1(cmdThread);
	t1.detach();

	while (g_bRun)
	{
		server.OnRun();
	}

	server.Close();
}

int main() {
	//test1();
	test2();

	printf("服务器已经退出\n");
	getchar();
	return 0;
}