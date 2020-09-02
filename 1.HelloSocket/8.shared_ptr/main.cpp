#include "Alloctor.h"
#include<iostream>
#include<stdlib.h>
#include<thread>
#include<mutex>
#include "CELLTimestamp.hpp"

using namespace std;

//感受:共享指针唯一的好处就是不用关心何时去delete,但是性能不行

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

//智能指针
void test1() {
	//普通使用
	ClassA* a1 = new ClassA;
	a1->num = 200;
	delete a1;
	
	//共享指针使用
	shared_ptr<ClassA> b = make_shared<ClassA>();
	b->num = 100;

	//共享指针赋值到普通指针
	ClassA * c = b.get();
}

void func2(ClassA* pA)
{
	pA->num++;
}

//共享指针 当做普通指针传入函数
void test2() {
	shared_ptr<ClassA> Pb = make_shared<ClassA>();
	cout << "引用计数:" << Pb.use_count() << endl;
	Pb->num = 100;

	func2(Pb.get());
	cout << Pb->num << endl;//101
}

void func3(shared_ptr<ClassA> pA)
{
	cout << "引用计数:" <<pA.use_count() << endl;
	pA->num++;
}

//输出:
/*
class A
引用计数 :1
引用计数 : 2
101
引用计数 : 2
~class A
*/
void test3()
{
	shared_ptr<ClassA> P1 = make_shared<ClassA>();
	cout << "引用计数:" << P1.use_count() << endl;
	P1->num = 100;

	func3(P1);
	cout << P1->num << endl;//102

	shared_ptr<ClassA> P2 = P1;
	cout << "引用计数:" << P1.use_count() << endl;
}

void func4(ClassA& pA)
{
	pA.num++;
}

void func4(shared_ptr<ClassA>& pA)
{
	pA->num++;
}

//说明共享指针 消耗更大
void test4()
{
	//耗时:1秒
	{
		shared_ptr<ClassA>b = make_shared<ClassA>(100);
		b->num = 200;
		CELLTimestamp tTime;
		for (int i = 0; i < 10000000; i++)
		{
			func4(b);
		}
		cout << tTime.getElapsedTimeInMilliSec() << endl;
	}

	//耗时:0.1秒
	{
		ClassA* b = new ClassA(100);
		b->num = 200;
		CELLTimestamp tTime;
		for (int i = 0; i < 10000000; i++)
		{
			func4(*b);
		}
		cout << tTime.getElapsedTimeInMilliSec() << endl;
	}
}

int main() 
{
	//test1();
	//test2();
	//test3();
	test4();

	system("pause");
}