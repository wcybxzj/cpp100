#include"EasyTcpClient.hpp"
#include<iostream>

//linux:make -f index.txt


class MyClient :public EasyTcpClient
{
public:
	virtual void OnNetMsg(netmsg_DataHeader *header)
	{
		switch (header->cmd)
		{
			case CMD_LOGIN_RESULT:
			{
				netmsg_LoginR* login = (netmsg_LoginR*)header;
			}
			break;

			case CMD_LOGOUT_RESULT:
			{
				netmsg_LogoutR*logout = (netmsg_LogoutR*)header;
			}
			break;
			
			case CMD_NEW_USER_JOIN:
			{
				netmsg_NewUserJoin* userJoin = (netmsg_NewUserJoin*)header;
			}
			break;

			case CMD_ERROR:
			{
				CELLLog::Info("<socket=%d> recv msgType:CMD_ERROR\n",
					(int)_pClient->sockfd());
			}
			break;

			default:
			{
				CELLLog::Info("error, <socket=%d> recv undefine msgType\n",
					(int)_pClient->sockfd());
			}
		}
	}
};

bool g_bRun = true;
//客户端总数量
const int cCount = 10;
//线程数量
const int tCount = 1;
EasyTcpClient* client[cCount];
//发送计数
std::atomic_int sendCount(0);
//继续线程计数:目标是让多个并发先的线程一起工作
std::atomic_int readyCount(0);


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
	CELLTimestamp t;
	while (g_bRun) {
		for (int i = begin; i < end; i++)
		{
			//接受超过了3秒 并且是第一个socket就不再接收数据
			//if (t.getElapsedSecond()> 3.0 && i == begin)
			//{
			//	continue;
			//}
			client[i]->OnRun();
		}
	}
}


//线程id:1-4
void sendThread(int id) {
	CELLLog::Info("thread<%d> start\n", id);
	int fd_nums = cCount / tCount;
	int begin = (id - 1) * fd_nums;
	int end = id * fd_nums;

	for (int i = begin; i < end; i++)
	{
		if (!g_bRun)
		{
			return;
		}
		client[i] = new MyClient();
	}

	for (int i = begin; i < end; i++)
	{
		if (!g_bRun)
		{
			return;
		}
		client[i]->Connect("127.0.0.1", 4567);
		//client[i]->Connect("65.49.211.61", 4567);//banwa
		//client[i]->Connect("39.97.236.187", 4567);//aliyun
		//client[i]->Connect("192.168.204.132", 4567);//ubuntu
	}

	CELLLog::Info("thread<%d>, connect fd range<begin:%d, end:%d>",
		id, begin, end);

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
			if (SOCKET_ERROR != client[i]->SendData(login))
			{
				sendCount++;
			}
			else
			{
				//printf("sendData 用户定义的输出缓冲满了\n");
				std::chrono::milliseconds t(99);
				std::this_thread::sleep_for(t);
			}
		}
	}

	for (int i = begin; i < end; i++)
	{
		client[i]->Close();
		delete client[i];
	}

	printf("thread<%d>, exit", id);
}

int main() {
	CELLLog::Instance().setLogPath("clientLog.txt", "w");

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
	CELLLog::Info("client已经退出\n");
	return 0;
}