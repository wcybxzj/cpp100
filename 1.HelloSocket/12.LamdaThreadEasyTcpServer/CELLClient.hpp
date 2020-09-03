#ifndef _CellClient_hpp_
#define _CellClient_hpp_

#include "CELL.hpp"
class CellClient
{
private:
	SOCKET _sockfd;

	//消息缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE];//100KB
	int _lastPos = 0;

	char _szSendBuf[SEND_BUFF_SIZE];
	int _lastSendPos = 0;


public:
	CellClient(SOCKET sockfd = INVALID_SOCKET) {
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, RECV_BUFF_SIZE);
		_lastPos = 0;

		memset(_szSendBuf, 0, SEND_BUFF_SIZE);
		_lastSendPos = 0;
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

	~CellClient() {
#ifdef _WIN32
		closesocket(_sockfd);
#else
		close(_sockfd);
#endif
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
				_lastSendPos = 0;

				ret = send(_sockfd, _szSendBuf, SEND_BUFF_SIZE, 0);
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
};
#endif