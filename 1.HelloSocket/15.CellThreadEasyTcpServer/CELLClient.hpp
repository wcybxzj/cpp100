#ifndef _CellClient_hpp_
#define _CellClient_hpp_

#include "CELL.hpp"

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

	//消息缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE];//100KB
	int _lastPos = 0;

	char _szSendBuf[SEND_BUFF_SIZE];
	int _lastSendPos = 0;

	//心跳死亡计时
	time_t _dtHeart;

	//上次发送消息数据的时间
	time_t _dtSend;
	

public:
	CellClient(SOCKET sockfd = INVALID_SOCKET) {
		static int n = 1;
		id = n++;

		_sockfd = sockfd;
		memset(_szMsgBuf, 0, RECV_BUFF_SIZE);
		_lastPos = 0;

		memset(_szSendBuf, 0, SEND_BUFF_SIZE);
		_lastSendPos = 0;

		resetDTHeart();
		resetDTSend();
	}

	~CellClient() {
		printf("s=%d CellClient%d.~CellClient\n", serverId, id);
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

	char* msgBuf() {
		return _szMsgBuf;
	}

	int getLastPos() {
		return _lastPos;
	}

	void setLastPos(int pos) {
		_lastPos = pos;
	}

	//立即发送数据
	int SendDataReal(netmsg_DataHeader* header)
	{
		SendData(header);
		SendDataReal();
	}

	//立即将发送缓冲区的数据发送给客户端
	int SendDataReal()
	{
		int ret = SOCKET_ERROR;
		//缓冲区有数据
		if (_lastSendPos > 0 && SOCKET_ERROR != _sockfd)
		{
			//发送数据
			ret = send(_sockfd, _szSendBuf, _lastSendPos, 0);
			//数据尾部位置清零
			_lastSendPos = 0;
			//
			resetDTSend();
		}
		return ret;
	}


	//发送指定socket数据
	//多个package合并到一起一次发送避免send调用次数过多
	int SendData(netmsg_DataHeader* header)
	{
		int ret = SOCKET_ERROR;
		int nSendLen = header->dataLength;
		const char* pSendData = (const char*)header;

		while (true)
		{
			if (_lastSendPos + nSendLen > SEND_BUFF_SIZE)
			{
				int nCopyLen = SEND_BUFF_SIZE - _lastSendPos;
				memcpy(_szSendBuf + _lastSendPos, pSendData, nCopyLen);

				pSendData += nCopyLen;
				nSendLen -= nCopyLen;
				
				ret = send(_sockfd, _szSendBuf, SEND_BUFF_SIZE, 0);
				_lastSendPos = 0;
				resetDTSend();
				if (ret == SOCKET_ERROR)
				{
					return ret;
				}
			}
			else
			{
				memcpy(_szSendBuf + _lastSendPos, pSendData, nSendLen);
				_lastSendPos += nSendLen;
				break;
			}

		}
		return ret;
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
			printf("checkHeart dead:socketfd=%d, time=%d\n", _sockfd, _dtHeart);
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