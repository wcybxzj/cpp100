#ifndef _ALLOCTOR_H_
#define _ALLOCTOR_H_

#include <sys/types.h>

void* operator new(size_t size);
void operator delete(void *p) noexcept;

void* operator new[](size_t size);
void operator delete[](void* p) noexcept;

void* mem_alloc(size_t size);
void mem_free(void* p);

#endif