#include"EasyTcpClient.hpp"
#include<iostream>

//#include"CELLTimestamp.hpp"

//linux:make -f index.txt

bool g_bRun = true;

//客户端总数量
const int cCount = 100;

//线程数量
const int tCount = 4;

EasyTcpClient* client[cCount];
//发送计数
std::atomic_int sendCount = 0;
//继续线程计数:目标是让多个并发先的线程一起工作
std::atomic_int readyCount = 0;

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

void recvThread(int begin, int end) {
	while (g_bRun) {
		for (int i = begin; i < end; i++)
		{
			client[i]->OnRun();
		}
	}
}

//线程id:1-4
void sendThread(int id) {
	printf("thread<%d> start", id);
	int fd_nums = cCount / tCount;
	int begin = (id - 1) * fd_nums;
	int end = id * fd_nums;

	for (int i = begin; i < end; i++)
	{
		if (!g_bRun)
		{
			return;
		}
		client[i] = new EasyTcpClient();
	}

	for (int i = begin; i < end; i++)
	{
		if (!g_bRun)
		{
			return;
		}
		client[i]->Connect("127.0.0.1", 4567);
		//client[i]->Connect("65.49.211.61", 4567);
		//client[i]->Connect("39.97.236.187", 4567);
		//client[i]->Connect("192.168.204.132", 4567);
	}

	printf("thread<%d>, connect fd range<begin:%d, end:%d>", id, begin, end);

	//所有线程都connect好后一起进行数据发送
	readyCount++;
	while (readyCount < tCount)
	{
		std::chrono::milliseconds t(1000);
		std::this_thread::sleep_for(t);
	}

	std::thread t1(recvThread, begin, end);
	t1.detach();

	netmsg_Login login[1];
	for (int i = 0; i < 1; i++)
	{
		strcpy(login[i].userName, "ybx");
		strcpy(login[i].PassWord, "12345");
	}

	int nLen = sizeof(login);
	while (g_bRun)
	{
		for (int i = begin; i < end; i++)
		{
			if (SOCKET_ERROR != client[i]->SendData(login, nLen))
			{
				sendCount++;
			}
			else
			{
				//printf("send errro\n");
				std::chrono::milliseconds t(1000000);
				std::this_thread::sleep_for(t);
			}
		}

		//std::chrono::seconds t(1);
		//std::this_thread::sleep_for(t);
	}

	for (int i = begin; i < end; i++)
	{
		client[i]->Close();
		delete client[i];
	}

	printf("thread<%d>, exit", id);
}

//定时测试
void test1() {
	CELLTimestamp t;

	while (1)
	{
		if (t.getElapsedSecond()>1.0)
		{
			std::cout << "11111111\n" << std::endl;
			t.update();
		}
	}
}


int main() {


	std::thread t1(cmdThread);
	t1.detach();

	for (int i = 0; i < tCount; i++)
	{
		std::thread t1(sendThread, i+1);
		t1.detach();
	}

	static int sendTotalCount = 0;
	CELLTimestamp tTime;
	while (g_bRun)
	{
		auto t = tTime.getElapsedSecond();
		if (t>=1.0)
		{
			sendTotalCount += sendCount;
			printf("thread<%d>, clients<%d>, time<%lf>,send package num:<%d>,sendTotalCount:<%d>\n", 
				tCount, cCount, t, (int)(sendCount/t), (int)(sendTotalCount));
			
			sendCount = 0;
			tTime.update();
		}
		//Sleep(1);
	}
	printf("client已经退出\n");
	return 0;
}