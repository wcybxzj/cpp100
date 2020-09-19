#include"EasyTcpClient.hpp"
#include"CELLMsgStream.hpp"
#include<iostream>

using namespace std;

class MyClient :public EasyTcpClient
{
public:
	virtual void OnNetMsg(netmsg_DataHeader*header)
	{
		switch (header->cmd)
		{
			case CMD_LOGOUT_RESULT:
			{
				CELLRecvStream r(header);
				auto n1 = r.ReadInt8();
				auto n2 = r.ReadInt16();
				auto n3 = r.ReadInt32();
				auto n4 = r.ReadFloat();
				auto n5 = r.ReadDouble();
				uint32_t n = 0;
				r.onlyRead(n);
				char name[32] = {};
				auto n6 = r.ReadArray(name, 32);
				char pw[32] = {};
				auto n7 = r.ReadArray(pw, 32);
				int ata[10] = {};
				auto n8 = r.ReadArray(ata, 10);

				cout << name << endl;

				CELLLog::Info("<socket>:%d recv: CMD_LOGOUT_RESULT\n",
					(int)_pClient->sockfd());
				break;
			}
			case CMD_ERROR:
			{
				CELLLog::Info("<socket>:%d recv: CMD_ERROR\n",
					(int)_pClient->sockfd());
				break;
			}
			default:
			{
				CELLLog::Info("<socket>:%d recv: undefined\n",
					(int)_pClient->sockfd());
				break;
			}
		}
	}
};

//CELLStream测试
void test1()
{
	CELLStream s(128);
	s.WriteInt8(11);
	s.WriteInt16(22);
	s.WriteInt32(33);
	s.WriteFloat(4.5f);
	s.WriteDouble(6.7);
	char arr1[] = "ahah";
	s.WriteArray(arr1, strlen(arr1));
	const char* arr2 = "heihei";
	s.WriteArray(arr2, strlen(arr2));
	int arr3[] = { 1,2,3,4,5 };
	s.WriteArray(arr3, 5);
	
	CELLStream s1(s.data(), s.length());
	auto n1 = s1.ReadInt8();
	printf("%d\n", n1);//11
	std::cout << n1 << std::endl;// [ 因为是uint8当成char输出了
	auto n2 = s1.ReadInt16();
	std::cout << n2 << std::endl;
	auto n3 = s1.ReadInt32();
	std::cout << n3 << std::endl;
	auto n4 = s1.ReadFloat();
	std::cout << n4 << std::endl;
	auto n5 = s1.ReadDouble();
	std::cout << n5 << std::endl;
	uint32_t nTmp = 0;
	s1.onlyRead(nTmp);
	std::cout << nTmp << std::endl;//4
	char arr4[32] = {};
	s1.ReadArray(arr4, 32);
	cout << arr4 << endl;

	char arr5[32] = {};
	s1.ReadArray(arr5, 32);
	cout << arr5 << endl;

	int arr6[32] = {};
	auto len6 = s1.ReadArray(arr6, 32);
	for (int i = 0; i < len6; i++)
	{
		cout << arr6[i] << endl;
	}
}

//client正式测试
void test2()
{
	CELLSendStream s(128);
	s.setNetCmd(CMD_LOGOUT);
	s.WriteInt8(1);
	s.WriteInt16(2);
	s.WriteInt32(3);
	s.WriteFloat(4.5f);
	s.WriteDouble(6.7);
	s.WriteString("client");
	char a[] = "ahah";
	s.WriteArray(a, strlen(a));
	int b[] = { 1,2,3,4,5 };
	s.WriteArray(b, 5);
	s.finish();

	MyClient client;
	client.Connect("127.0.0.1", 4567);

	while (client.isRun())
	{
		client.OnRun();
		client.SendData(s.data(), s.length());
		CELLThread::Sleep(10);
	}
}

int main()
{
	//test1();
	test2();
}