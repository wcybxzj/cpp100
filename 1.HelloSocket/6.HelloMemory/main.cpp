#include "Alloctor.h"
#include<iostream>
#include<stdlib.h>
#include<thread>
#include<mutex>
#include "CELLTimestamp.hpp"

using namespace std;
mutex m;

const int tCount = 8;
const int mCount = 100000;
const int nCount = mCount/tCount;

void workFun(int index)
{
	char* data[nCount];
	for (int i = 0; i < nCount; i++)
	{
		data[i] = new char[(rand()%128)+1];
	}

	for (int i = 0; i < nCount; i++)
	{
		delete[] data[i];
	}
}


//多线程测试 改写的new 和 delete
void test2() {
	thread t[tCount];
	for (int i = 0; i < tCount; i++)
	{
		t[i] = thread(workFun, i);
	}

	CELLTimestamp tTime;
	for (int i = 0; i < tCount; i++)
	{
		t[i].join();
	}
	cout << tTime.getElapsedTimeInMilliSec() << endl;
	cout << "Hello, main thread." << endl;

}


//重写new delete 测试
void test1() {
	int a = 123;
	size_t b = 123;
	int* pa = new int(a);
	int* pb = new int(b);

	////1
	//char* data1 = new char[128];
	//delete[] data1;
	////2
	//char* data2 = new char;
	//delete data2;
	////3
	//char* data3 = (char*)malloc(64);
	//free(data3);
}


class ClassA
{
public:
	int num = 0;

	ClassA(int n = 0)
	{
		num = n;
		printf("class A\n");
	}

	~ClassA()
	{
		printf("~class A\n");
	}
};

//智能指针测试
void test3() {
	ClassA* a1 = new ClassA;
	a1->num = 200;
	delete a1;
	
	shared_ptr<ClassA> b = make_shared<ClassA>();
	b->num = 100;

	ClassA * c = b.get();
}

int main() {
	//test1();
	
	//test2();

	test3();

	system("pause");

}