#ifndef _MemoryMgr_hpp_
#define _MemoryMgr_hpp_
#include <stdlib.h>
#include<assert.h>
#include "assert.h"
#include<mutex>
#include<stdio.h>

//#define DEBUG

//_DEBUG ��visual studio��debugģʽʱ��ĺ�,��Ŀ���Կ����ҵ�
#ifdef DEBUG
#include<stdlib.h>
	#define xPrintf(...) printf(__VA_ARGS__)
#else
	#define xPrintf(...)
#endif

//���������
#include <cassert>

#define MAX_MEMORY_SIZE 128

class MemoryAlloc;

//�ڴ���еĿ�
class MemoryBlock
{
public:
	//�����ǂ��ڴ��
	MemoryAlloc* pAlloc;
	//��һ����
	MemoryBlock* pNext;
	//��id
	int nID;
	//���ô���
	int nRef;
	//����ڴ���Ƿ����ڴ����
	bool bPool;
private:
	char c1;
	char c2;
	char c3;
};

//�ڴ��
class MemoryAlloc
{
protected:
	//�ڴ�ص�ַ
	char* _pBuf;
	//��ǰ�ڴ���п��õ��ڴ��
	MemoryBlock* _pHeader;
	//�ڴ��Ĵ�Сdd
	int _nSize;
	//�ڴ��ĸ���
	int _nBlockSize;
	std::mutex _mutex;

public:
	MemoryAlloc()
	{
		_pBuf = nullptr;
		_pHeader = nullptr;
		_nSize = 0;
		_nBlockSize = 0;
		xPrintf("MemoryAlloc\n");
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
		xPrintf("initMemory _nSize:%d, _nBlockSize:%d\n",_nSize, _nBlockSize);

		assert(nullptr == _pBuf);
		
		if (_pBuf)
		{
			return;
		}
		//ÿ�����С
		int realSize = _nSize + sizeof(MemoryBlock);
		//�ڴ�ش�С
		int bufSize = realSize * _nBlockSize;
		_pBuf = (char*)malloc(bufSize);

		//��ʼ���ڴ��
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
			printf("memory pool �ľ� wanted nSize:%d\n", nSize);
			pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
			pReturn->bPool = false;
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->pAlloc = nullptr;
			pReturn->pNext = nullptr;
		}
		else
		{
			pReturn = _pHeader;
			_pHeader = _pHeader->pNext;
			assert(0 == pReturn->nRef);
			pReturn->nRef = 1;
		}
		xPrintf("allocMem=%llx, id=%d, size=%d\n", pReturn, pReturn->nID, nSize);
		return (((char*)pReturn) + sizeof(MemoryBlock));
	}

	void freeMemory(void* pMem)
	{
		MemoryBlock* pBlock = 
				(MemoryBlock*)((char *)pMem - sizeof(MemoryBlock));

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
		xPrintf("allocMem=%llx, id=%d, size=%d\n", pBlock, pBlock->nID, pBlock->pAlloc->_nBlockSize);
	}

};

template<int nSize, int nBlockSize>
class MemoryAlloctor :public MemoryAlloc
{
public:
	MemoryAlloctor() {
		//��ǰƽָ̨���С
		const int n = sizeof(void*);
		//ÿ���ڴ��Ҫ����ָ���С�ı�������
		//���� nSize����61 Ҫת����64
		// 61/8 = 7 61%8 = 5

		_nSize = (nSize / n) * n + (nSize % n ? n : 0);
		_nBlockSize = nBlockSize;
	}
};

class MemoryMgr {
private:
	MemoryAlloctor<64,	1000000> _mem64;
	MemoryAlloctor<128, 1000000> _mem128;
	//MemoryAlloctor<256, 100000> _mem256;
	//MemoryAlloctor<512, 100000> _mem512;
	//MemoryAlloctor<1024, 100000> _mem1024;
	MemoryAlloc* _szAlloc[MAX_MEMORY_SIZE + 1];//1025


	MemoryMgr() {
		init_szAlloc(0, 64, &_mem64);
		init_szAlloc(65, 128, &_mem128);
		//init_szAlloc(129, 256, &_mem256);
		//init_szAlloc(257, 512, &_mem512);
		//init_szAlloc(513, 1024, &_mem1024);
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
	}

	void addRef(void* pMem)
	{
		MemoryBlock* pBlock = (MemoryBlock*) ((char*)pMem- sizeof(MemoryBlock));
		++pBlock->nRef;
	}
};

#endif