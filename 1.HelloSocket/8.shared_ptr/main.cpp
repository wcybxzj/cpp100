#include "Alloctor.h"
#include<iostream>
#include<stdlib.h>
#include<thread>
#include<mutex>
#include "CELLTimestamp.hpp"

using namespace std;

//����:����ָ��Ψһ�ĺô����ǲ��ù��ĺ�ʱȥdelete,�������ܲ���

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

//����ָ��
void test1() {
	//��ͨʹ��
	ClassA* a1 = new ClassA;
	a1->num = 200;
	delete a1;
	
	//����ָ��ʹ��
	shared_ptr<ClassA> b = make_shared<ClassA>();
	b->num = 100;

	//����ָ�븳ֵ����ָͨ��
	ClassA * c = b.get();
}

void func2(ClassA* pA)
{
	pA->num++;
}

//����ָ�� ������ָͨ�봫�뺯��
void test2() {
	shared_ptr<ClassA> Pb = make_shared<ClassA>();
	cout << "���ü���:" << Pb.use_count() << endl;
	Pb->num = 100;

	func2(Pb.get());
	cout << Pb->num << endl;//101
}

void func3(shared_ptr<ClassA> pA)
{
	cout << "���ü���:" <<pA.use_count() << endl;
	pA->num++;
}

//���:
/*
class A
���ü��� :1
���ü��� : 2
101
���ü��� : 2
~class A
*/
void test3()
{
	shared_ptr<ClassA> P1 = make_shared<ClassA>();
	cout << "���ü���:" << P1.use_count() << endl;
	P1->num = 100;

	func3(P1);
	cout << P1->num << endl;//102

	shared_ptr<ClassA> P2 = P1;
	cout << "���ü���:" << P1.use_count() << endl;
}

void func4(ClassA& pA)
{
	pA.num++;
}

void func4(shared_ptr<ClassA>& pA)
{
	pA->num++;
}

//˵������ָ�� ���ĸ���
void test4()
{
	//��ʱ:1��
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

	//��ʱ:0.1��
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