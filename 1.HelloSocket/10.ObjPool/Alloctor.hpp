#ifndef _ALLOCTOR_HPP_
#define _ALLOCTOR_HPP_
#include <stdlib.h>
#include"MemoryMgr.hpp"
#include <iostream>

//void* operator new(size_t size);
//void operator delete(void* p);
//
//void* operator new[](size_t size);
//void operator delete[](void* p);
//
//void* mem_alloc(size_t size);
//void mem_free(void* p);

void* operator new(size_t nSize)
{
	//std::cout << "operator new()" << std::endl;
	return MemoryMgr::Instance().allocMem(nSize);
}

void operator delete(void* p)
{
	MemoryMgr::Instance().freeMem(p);
}

void* operator new[](size_t nSize)
{
	return MemoryMgr::Instance().allocMem(nSize);
}

void operator delete[](void* p)
{
	MemoryMgr::Instance().freeMem(p);
}

void* mem_alloc(size_t size) {
	return malloc(size);
}

void mem_free(void* p) {
	free(p);
}

#endif