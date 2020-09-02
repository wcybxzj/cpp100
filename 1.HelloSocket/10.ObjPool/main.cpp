#include "Alloctor.h"
#include<stdlib.h>
#include<iostream>
#include<thread>
#include<mutex>//锁
#include<memory>
#include"CELLTimestamp.hpp"
#include"CELLObjectPool.hpp"

using namespace std;

class ClassA :public ObjectPoolBase<ClassA, 1000000>
{
public:
	int num = 0;

	ClassA(int n)
	{
		num = n;
		
		//printf("init ClassA\n");
	}

	~ClassA()
	{
		//printf("~ ClassA\n");
	}
};

class ClassB :public ObjectPoolBase<ClassB, 10>
{
public:
	int num = 0;

	ClassB(int n, int m)
	{
		num = n*m;
		//xPrintf("init ClassB\n");
	}

	~ClassB()
	{
		//xPrintf("destroy ClassB\n");
	}
};

void test1()
{
	ClassA* p1 = new ClassA(5);
	delete p1;

	ClassA* p2 = ClassA::createObject(6);
	ClassA::destroyObject(p2);

	ClassB* b1 = new ClassB(11, 22);
	delete b1;

	ClassB* b2 = ClassB::createObject(33, 44);
	ClassB::destroyObject(b2);
}


const int tCount = 8;
const int mCount = 1000000;
const int nCount = mCount / tCount;

void workFun(int index)
{
	ClassA* data[nCount];
	for (size_t i = 0; i < nCount; i++)
	{
		data[i] = ClassA::createObject(6);//每个 ClassA 4字节
	}
	for (size_t i = 0; i < nCount; i++)
	{
		ClassA::destroyObject(data[i]);
	}
}

void test2()
{
	CELLTimestamp tTime;

	thread t[tCount];
	for (int n = 0; n < tCount; n++)
	{
		t[n] = thread(workFun, n);//每次申请16字节
	}

	for (int n = 0; n < tCount; n++)
	{
		t[n].join();
	}
	cout << tTime.getElapsedSecond() << endl;
	cout << "Hello,main thread." << endl;
}


int main()
{
	//test1();
	test2();
}


