//linux:make -f index.txt
#include"EasyTcpServer.hpp"
#include "CELLMsgStream.hpp"

#include<iostream>
using namespace std;

bool g_bRun = true;

class MyServer : public EasyTcpServer
{
public:

	//ֻ�ᱻһ���̴߳��� ��ȫ
	virtual void OnNetJoin(CellClient* pClient)
	{
		EasyTcpServer::OnNetJoin(pClient);
	}

	//cellServer 4 ����̴߳��� ����ȫ
	//���ֻ����1��cellServer���ǰ�ȫ��
	virtual void OnNetLeave(CellClient* pClient)
	{
		EasyTcpServer::OnNetLeave(pClient);
	}

	virtual void OnNetMsg(CellServer* pCellServer, CellClient* pClient, netmsg_DataHeader* header)
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
			//������body���ֶ��뵽szRecv
			login = (netmsg_Login*)header;
			//printf("�ͻ���:%d,�յ�����:CMD_LOGIN, len:%d, username:%s, password:%s\n",
			//	cSock, login->dataLength, login->userName, login->PassWord);

			netmsg_LoginR ret;
			if (pClient->SendData(&ret) == SOCKET_ERROR)
			{
				CELLLog::Info("<Socket=%d> �û����巢�ͻ���������!\n", pClient->sockfd());
			}
			//netmsg_LoginR* ret = new netmsg_LoginR();
			//pCellServer->addSendTask(pClient, ret);
			break;
		}

		case CMD_LOGOUT:
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

			//
			CELLSendStream s(128);
			s.setNetCmd(CMD_LOGOUT_RESULT);
			s.WriteInt8(1);
			s.WriteInt16(2);
			s.WriteInt32(3);
			s.WriteFloat(4.5f);
			s.WriteDouble(6.7);
			const char* str = "sever";
			s.WriteArray(str, strlen(str));
			char a[] = "ahah";
			s.WriteArray(a, strlen(a));
			int b[] = { 1,2,3,4,5 };
			s.WriteArray(b, 5);
			s.finish();
			pClient->SendData(s.data(), s.length());
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
			CELLLog::Info("<socket=%d>�յ�δ������Ϣ,���ݳ��ȣ�%d\n", pClient->sockfd(), header->dataLength);
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
			CELLLog::Info("�˳�cmdThread�߳�!!!!\n");
			break;
		}
		else
		{
			CELLLog::Info("��֧�ֵ�����\n");
		}
	}

	CELLLog::Info("���˳���\n");
	//system("pause");

}

int main() {
	test1();
	return 0;
}