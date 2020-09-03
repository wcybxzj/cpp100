#include"Alloctor.h"
#include <stdlib.h>
#include"MemoryMgr.hpp"
#include <iostream>
#include<cstddef>

void* operator new(std::size_t nSize)
{
	//std::cout << "operator new()" << std::endl;
	return MemoryMgr::Instance().allocMem(nSize);
}

void operator delete(void* p)
{
	MemoryMgr::Instance().freeMem(p);
}

void* operator new[](std::size_t nSize)
{
	//std::cout << "operator new[]" << std::endl;
	return MemoryMgr::Instance().allocMem(nSize);
}

void operator delete[](void* p)
{
	MemoryMgr::Instance().freeMem(p);
}

void* mem_alloc(std::size_t size) {
	return malloc(size);
}

void mem_free(void* p) {
	free(p);
}

