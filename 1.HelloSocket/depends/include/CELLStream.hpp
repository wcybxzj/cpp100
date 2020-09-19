#ifndef _CELL_STREAM_HPP_
#define  _CELL_STREAM_HPP_

#include<cstdint>

class CELLStream
{
private:
	char* _pBuff = nullptr;
	int _nSize = 0;
	int _nWritePos = 0;
	int _nReadPos = 0;
	bool _bDelete = true;

public:
	CELLStream(char* pData, int nSize, bool bDelete = false)
	{
		_nSize = nSize;
		_pBuff = pData;
		_bDelete = bDelete;
	}

	CELLStream(int nSize = 1024)
	{
		_nSize = nSize;
		_pBuff = new char[_nSize];
		_bDelete = true;
	}


	virtual ~CELLStream()
	{
		if (_bDelete && _pBuff)
		{
			delete[] _pBuff;
			_pBuff = nullptr;
		}
	}

public:
	char* data()
	{
		return _pBuff;
	}

	int length()
	{
		return _nWritePos;
	}

	inline bool canRead(int n)
	{
		return _nSize - _nReadPos >= n;
	}


	inline bool canWrite(int n)
	{
		return _nSize - _nWritePos >= n;
	}

	inline void push(int n)
	{
		_nWritePos += n;
	}

	inline void pop(int n)
	{
		_nReadPos += n;
	}

	inline void setWritePos(int n)
	{
		_nWritePos = n;
	}



	//////////////////Read////////////////////////
	template<typename T>
	bool Read(T& n, bool bOffset = true)
	{
		auto nLen = sizeof(T);
		if (canRead(nLen))
		{
			memcpy(&n, _pBuff + _nReadPos, nLen);
			if (bOffset)
			{
				pop(nLen);
			}
			return true;
		}
		return false;
	}

	template<typename T>
	bool onlyRead(T& n) {
		return Read(n, false);
	}
	
	template<typename T>
	uint32_t ReadArray(T* pArr, uint32_t len)
	{
		//获取当前实际内存个数字段
		uint32_t realLen;
		Read(realLen, false);
		//外部的缓存可以存入
		if (realLen < len)
		{
			auto nLen = realLen * sizeof(T);
			//判断当前stream中是否有足够的数据
			if (canRead(nLen + sizeof(uint32_t)))
			{
				pop(sizeof(uint32_t));
				memcpy(pArr, _pBuff + _nReadPos, nLen);
				pop(nLen);
				return realLen;
			}
		}
		return 0;
	}
	
	int8_t ReadInt8(int8_t def = 0)
	{
		Read(def);
		return def;
	}

	int16_t ReadInt16(int16_t def = 0)
	{
		Read(def);
		return def;
	}

	int32_t ReadInt32(int32_t def = 0)
	{
		Read(def);
		return def;
	}

	float ReadFloat(float def = 0.0f)
	{
		Read(def);
		return def;
	}

	double ReadDouble(double def = 0.0)
	{
		Read(def);
		return def;
	}

	//////////////////Write////////////////////////
	template<typename T>
	bool Write(T n)
	{
		auto nLen = sizeof(T);
		if (canWrite(nLen))
		{
			memcpy(_pBuff + _nWritePos, &n, nLen);
			push(nLen);
			return true;
		}
		return false;
	}

	template<typename T>
	bool WriteArray(T* pData, uint32_t len)
	{
		auto nLen = sizeof(T) * len;
		if (canWrite(nLen + sizeof(uint32_t)))
		{
			Write(len);
			memcpy(_pBuff + _nWritePos, pData, nLen);
			push(nLen);
			return true;
		}
		return false;
	}

	bool WriteInt8(int8_t n)
	{
		return Write(n);
	}

	bool WriteInt16(int16_t n)
	{
		return Write(n);
	}

	bool WriteInt32(int32_t n)
	{
		return Write(n);
	}

	bool WriteFloat(float n)
	{
		return Write(n);
	}

	bool WriteDouble(double n)
	{
		return Write(n);
	}


};





#endif