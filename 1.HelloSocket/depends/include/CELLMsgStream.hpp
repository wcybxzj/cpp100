#ifndef _CELL_MSG_STREAM_HPP_
#define _CELL_MSG_STREAM_HPP_
#include "CELLStream.hpp"
#include"MessageHeader.hpp"

class CELLRecvStream :public CELLStream
{
public:
	CELLRecvStream(netmsg_DataHeader*header):
		CELLStream((char*)header, header->dataLength)
	{
		push(header->dataLength);
		//预读消息总长度
		ReadInt16();
		
		//预读命令
		getNetCmd();
	}

	int16_t getNetCmd()
	{
		int16_t cmd = CMD_ERROR;
		Read(cmd);
		return cmd;
	}
};

class CELLSendStream :public CELLStream
{
public:
	CELLSendStream(char* pData, int nSize, bool bDelete = false)
		:CELLStream(pData, nSize, bDelete)
	{
		Write((int16_t)0);
	}

	CELLSendStream(int nSize = 1024)
		:CELLStream(nSize)
	{
		Write<uint16_t>(0);
	}

	void setNetCmd(int16_t cmd)
	{
		Write<uint16_t>(cmd);
	}

	bool WriteString(const char* str, int len)
	{
		return WriteArray(str, len);
	}

	bool WriteString(const char* str)
	{
		return WriteArray(str, strlen(str));
	}

	bool WriteString(std::string& str)
	{
		return WriteArray(str.c_str(), str.length());
	}

	void finish()
	{
		int pos = length();
		setWritePos(0);
		Write<uint16_t>(pos);
		setWritePos(pos);
	}

};

#endif