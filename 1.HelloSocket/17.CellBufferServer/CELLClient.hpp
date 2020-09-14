#ifndef _CellClient_hpp_
#define _CellClient_hpp_

#include "CELL.hpp"
#include "CELLBuffer.hpp"

//
//#define CLIENT_HEART_DEAD_TIME 60000 //60s
#define CLIENT_HEART_DEAD_TIME 6000//6s

//在间隔指定时间后
//把发送缓冲区内缓存的消息数据发送给客户端
#define CLIENT_SEND_BUFF_TIME 200

class CellClient
{
public:
	int id = -1;
	int serverId = -1;

private:
	SOCKET _sockfd;
	CELLBuffer _recvBuff;
	CELLBuffer _sendBuff;

	//心跳死亡计时
	time_t _dtHeart;
	//上次发送消息数据的时间
	time_t _dtSend;

public:
	CellClient(SOCKET sockfd = INVALID_SOCKET):
		_sendBuff(SEND_BUFF_SIZE),
		_recvBuff(RECV_BUFF_SIZE)
	{
		static int n = 1;
		id = n++;
		_sockfd = sockfd;

		resetDTHeart();
		resetDTSend();
	}

	~CellClient() {
		CELLLog::Info("s=%d CellClient%d.~CellClient\n", serverId, id);
		if (INVALID_SOCKET != _sockfd)
		{
#ifdef _WIN32
			closesocket(_sockfd);
#else
			close(_sockfd);
#endif
		}
	}

	SOCKET sockfd() {
		return _sockfd;
	}

	int RecvData()
	{
		return _recvBuff.read4socket(_sockfd);
	}

	bool hasMsg()
	{
		return _recvBuff.hasMsg();
	}

	netmsg_DataHeader* front_msg()
	{
		return (netmsg_DataHeader*)_recvBuff.data();
	}

	void pop_front_msg()
	{
		if (hasMsg())
		{
			_recvBuff.pop(front_msg()->dataLength);
		}
	}

	//立即将发送缓冲区的数据发送给客户端
	int SendDataReal()
	{
		resetDTSend();
		return _sendBuff.write2socket(_sockfd);
	}

	//发送指定socket数据
	//多个package合并到一起一次发送避免send调用次数过多
	int SendData(netmsg_DataHeader* header)
	{
		if (_sendBuff.push((const char*)header, header->dataLength))
		{
			return header->dataLength;
		}
		return SOCKET_ERROR;
	}

	void resetDTHeart()
	{
		_dtHeart = 0;
	}
	
	void resetDTSend()
	{
		_dtSend = 0;
	}


	bool checkHeart(time_t dt)
	{
		_dtHeart += dt;
		if (_dtHeart >= CLIENT_HEART_DEAD_TIME) {
			CELLLog::Info("checkHeart dead:socketfd=%d, time=%d\n", _sockfd, _dtHeart);
			return true;
		}
		return false;
	}

	//定时发送消息检测
	bool checkSend(time_t dt)
	{
		_dtSend += dt;
		if (_dtSend >= CLIENT_SEND_BUFF_TIME)
		{
			//printf("定时发送checkSend:s=%d,time=%d\n", _sockfd, _dtSend);
			//立即将发送缓冲区的数据发送出去
			SendDataReal();
			//重置发送计时
			resetDTSend();
			return true;
		}
		return false;
	}
};
#endif