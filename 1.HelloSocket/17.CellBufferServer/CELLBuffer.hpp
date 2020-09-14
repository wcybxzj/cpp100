#ifndef _CELL_BUFFER_HPP_
#define _CELL_BUFFER_HPP_

#include "CELL.hpp"

class CELLBuffer
{
private:
	char* _pBuff = nullptr;
	int _nLast = 0;
	int _nSize = 0;
	//д������
	int _fullCount = 0;


public:
	CELLBuffer(int  nSize=8192)
	{
		_nSize = nSize;
		_pBuff = new char[_nSize];
	}

	~CELLBuffer()
	{
		if (_pBuff)
		{
			delete[] _pBuff;
			_pBuff = nullptr;
		}
	}

	char* data()
	{
		return _pBuff;
	}

	bool push(const char* pData, int nLen)
	{
		if (_nLast+nLen <= _nSize)
		{
			memcpy(_pBuff + _nLast, pData, nLen);
			_nLast += nLen;

			if (_nLast == SEND_BUFF_SIZE)
			{
				++_fullCount;
			}

			return true;
		}
		
		++_fullCount;
		return  false;
	}

	void pop(int nLen)
	{
		//pop��ʣ�೤��
		int n = _nLast - nLen;
		//�������ʣ��
		if (n > 0)
		{
			memcpy(_pBuff, _pBuff + nLen, n);
		}
		
		//��������λ��
		_nLast = n;
		//����������
		if (_fullCount > 0)
		{
			_fullCount--;
		}
	}

	int write2socket(SOCKET sockfd) {
		int ret = 0;
		if (_nLast > 0 && INVALID_SOCKET != sockfd)
		{
			ret = send(sockfd, _pBuff, _nLast, 0);
			_nLast = 0;
			_fullCount = 0;
		}
		return ret;
	}

	int read4socket(SOCKET sockfd)
	{
		//�пռ���ж�ȡ
		if (_nSize - _nLast > 0)
		{
			char* buf = _pBuff + _nLast;
			int nLen = recv(sockfd, buf, _nSize - _nLast, 0);
			if (nLen <=0)
			{
				return nLen;
			}
			_nLast += nLen;
			return nLen;

		}
		return 0;
	}

	bool hasMsg()
	{
		if (_nLast >= sizeof(netmsg_DataHeader))
		{
			netmsg_DataHeader* header = (netmsg_DataHeader*)_pBuff;
			return _nLast >= header->dataLength;
		}
		return false;
	}

};

#endif