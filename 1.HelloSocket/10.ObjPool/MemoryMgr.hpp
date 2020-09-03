#ifndef _MemoryMgr_hpp_
#define _MemoryMgr_hpp_
#include <stdlib.h>
#include<assert.h>
#include<mutex>
#include<iostream>

#define DEBUG
#ifdef DEBUG
#include<stdlib.h>
	#define xPrintf(...) printf(__VA_ARGS__)
#else
	#define xPrintf(...)
#endif
#include <cassert>

#define MAX_MEMORY_SIZE 1024

class MemoryAlloc;

//内存池中的块 32字节
class MemoryBlock
{
public:
	//所属那個内存池
	MemoryAlloc* pAlloc;
	//下一个块
	MemoryBlock* pNext;
	//块id
	int nID;
	//引用次数
	int nRef;
	//这个内存块是否在内存池中
	bool bPool;
private:
	char c1;
	char c2;
	char c3;
};
//内存池
class MemoryAlloc
{
protected:
	//内存池地址
	char* _pBuf;
	//当前内存池中可用的内存块
	MemoryBlock* _pHeader;
	//内存块的大小dd
	int _nSize;
	//内存块的个数
	int _nBlockSize;
	std::mutex _mutex;

public:
	MemoryAlloc()
	{
		_pBuf = nullptr;
		_pHeader = nullptr;
		_nSize = 0;
		_nBlockSize = 0;
		xPrintf("memory pool:MemoryAlloc\n");
	}

	~MemoryAlloc()
	{
		if (_pBuf)
		{
			free(_pBuf);
		}
	}

	void initMemory()
	{
		xPrintf("memory pool:initMemory _nSize:%d, _nBlockSize:%d\n",
			_nSize, _nBlockSize);

		assert(nullptr == _pBuf);
		if (_pBuf)
		{
			return;
		}
		//每个块大小
		int realSize = _nSize + sizeof(MemoryBlock);
		//内存池大小
		int bufSize = realSize * _nBlockSize;
		_pBuf = (char*)malloc(bufSize);

		//初始化内存池
		_pHeader = (MemoryBlock*)_pBuf;
		_pHeader->bPool = true;
		_pHeader->nID = 0;
		_pHeader->nRef = 0;
		_pHeader->pAlloc = this;
		_pHeader->pNext = nullptr;

		MemoryBlock* pTemp1 = _pHeader;

		for (size_t i = 1; i < _nBlockSize; i++)
		{
			MemoryBlock* pTemp2 = (MemoryBlock*)(_pBuf + (i * realSize));
			pTemp2->bPool = true;
			pTemp2->nID = i;
			pTemp2->nRef = 0;
			pTemp2->pAlloc = this;
			pTemp2->pNext = nullptr;
			pTemp1->pNext = pTemp2;
			pTemp1 = pTemp2;
		}
	}

	void* allocMemory(int nSize)
	{
		std::lock_guard<std::mutex> lg(_mutex);
		if (!_pBuf)
		{
			initMemory();
		}
		
		MemoryBlock* pReturn = nullptr;
		if (nullptr == _pHeader)
		{
			pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
			pReturn->bPool = false;
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->pAlloc = nullptr;
			pReturn->pNext = nullptr;
			//xPrintf("memory pool:memory pool耗尽 allocMemory:%llx  id:%d, size:%d\n", pReturn, pReturn->nID, nSize);
		}
		else
		{
			pReturn = _pHeader;
			_pHeader = _pHeader->pNext;
			assert(0 == pReturn->nRef);
			pReturn->nRef = 1;
		}

		xPrintf("memory pool:allocMemory:%llx  id:%d, size:%d\n",pReturn, pReturn->nID, nSize);
		return (((char*)pReturn) + sizeof(MemoryBlock));
	}

	void freeMemory(void* pMem)
	{
		MemoryBlock* pBlock = 
				(MemoryBlock*)((char *)pMem - sizeof(MemoryBlock));
		xPrintf("memory pool:freeMemory:%llx\n", pBlock);

		assert(1 == pBlock->nRef);
		
		if (pBlock->bPool)
		{
			std::lock_guard<std::mutex> lg(_mutex);

			if ((--pBlock->nRef) != 0)
			{
				return;
			}

			pBlock->pNext = _pHeader;
			_pHeader = pBlock;
		}
		else
		{
			if ((--pBlock->nRef) != 0)
			{
				return;
			}
			free(pBlock);
		}
	}

};

template<int nSize, int nBlockSize>
class MemoryAlloctor :public MemoryAlloc
{
public:
	MemoryAlloctor() {
		//当前平台指针大小
		const int n = sizeof(void*);
		//每个内存块要按照指针大小的倍数生成
		//例如 nSize传入61 要转换成64
		// 61/8 = 7 61%8 = 5

		_nSize = (nSize / n) * n + (nSize % n ? n : 0);
		_nBlockSize = nBlockSize;

		xPrintf("memory pool:MemoryAlloctor _nSize:%d, _nBlockSize:%d\n", _nSize, _nBlockSize);
	}
};

class MemoryMgr {
private:
	MemoryAlloctor<64, 100000> _mem64;
	MemoryAlloctor<128, 100000> _mem128;
	MemoryAlloctor<256, 100000> _mem256;
	MemoryAlloctor<512, 100000> _mem512;
	MemoryAlloctor<1024, 100000> _mem1024;
	MemoryAlloc* _szAlloc[MAX_MEMORY_SIZE + 1];//1025


	MemoryMgr() {
		init_szAlloc(0, 64, &_mem64);
		init_szAlloc(65, 128, &_mem128);
		init_szAlloc(129, 256, &_mem256);
		init_szAlloc(257, 512, &_mem512);
		init_szAlloc(513, 1024, &_mem1024);
	}

	~MemoryMgr() {

	}

	void init_szAlloc(int nBegin, int nEnd, MemoryAlloc* pMemA)
	{
		for (int i = nBegin; i <= nEnd; i++)
		{
			_szAlloc[i] = pMemA;
		}
	}

public:
	static MemoryMgr& Instance()
	{
		static MemoryMgr mgr;
		return mgr;
	}

	void* allocMem(int nSize)
	{
		//xPrintf("MemoryMgr()->allocMem: nSize:%d\n", nSize);

		if (nSize <= MAX_MEMORY_SIZE)
		{
			return _szAlloc[nSize]->allocMemory(nSize);
		}
		else 
		{
			MemoryBlock* pReturn =
				(MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
			pReturn->bPool = false;
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->pAlloc = nullptr;
			pReturn->pNext = nullptr;
			return (char*)pReturn + sizeof(MemoryBlock);
		}
	}

	void freeMem(void* pMem)
	{
		MemoryBlock* pBlock =
			(MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		if (pBlock->bPool)
		{
			pBlock->pAlloc->freeMemory(pMem);
		}
		else {
			if (--pBlock->nRef == 0)
			{
				free(pBlock);
			}
		}
		//printf("MemoryMgr()->freeMem(): address:%x\n", pMem);
	}

	void addRef(void* pMem)
	{
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem- sizeof(MemoryBlock));
		++pBlock->nRef;
	}
};

#endif