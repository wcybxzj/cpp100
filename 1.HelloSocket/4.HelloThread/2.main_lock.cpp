#if 0
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<ctime>
#include<string>
#include<memory>
#include<stdexcept>
#include<iomanip>
#include<fstream>
#include<vector>
#include<list>
#include<stack>
#include<queue>
#include<set>
#include<map>
#include<algorithm>
#include<functional>
#include<iterator>
#include <numeric>
#include <future>                                                                                                                  
#include <cmath>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <atomic>
#include <mutex>
#include<iostream>
#include<thread>
#include<mutex>//��
#include<atomic>//ԭ��
#include "CELLTimestamp.hpp"

using namespace std;

mutex m;
int sum = 0;
const int tCount = 4;

//д��1��
//atomic_int sum2 = 0;
//д��2��
atomic<int> sum2 = 0;

//��ͨ������
void func4() {
	for (int i = 0; i < 10000000; i++)
	{
		m.lock();
		sum++;
		m.unlock();
	}
}

void test4() {
	sum = 0;
	thread t[tCount];
	for (int i = 0; i < tCount; i++)
	{
		t[i] = thread(func4);
	}

	CELLTimestamp tTime;

	for (int i = 0; i < tCount; i++)
	{
		t[i].join();
	}
	cout << "��ʱ:������):" << tTime.getElapsedTimeInMilliSec() << endl;
	cout << sum << endl;
}

//�Խ���:ֻҪ�������г���{} �Զ�����
void func5() {
	for (int i = 0; i < 10000000; i++)
	{
		lock_guard<mutex> lg(m);
		sum++;
	}
}

void test5() {
	sum = 0;
	thread t[tCount];
	for (int i = 0; i < tCount; i++)
	{
		t[i] = thread(func5);
	}

	CELLTimestamp tTime;

	for (int i = 0; i < tCount; i++)
	{
		t[i].join();
	}
	cout << "��ʱ:������):" << tTime.getElapsedTimeInMilliSec() << endl;
	cout << sum << endl;
}

//ԭ�ӱ���:
void func6() {
	for (int i = 0; i < 10000000; i++)
	{
		sum2++;
	}
}

/*

��ʱ:������):1356.46(���߳�+ԭ��int ��ʱ����)
40000000
��ʱ : ������):1424.43
40000000
*/
void test6() {
	sum2 = 0;
	thread t[tCount];
	for (int i = 0; i < tCount; i++)
	{
		t[i] = thread(func6);
	}

	CELLTimestamp tTime;

	for (int i = 0; i < tCount; i++)
	{
		t[i].join();
	}
	cout << "��ʱ:������):" << tTime.getElapsedTimeInMilliSec() << endl;
	cout << sum2 << endl;


	tTime.update();
	sum2 = 0;
	for (int i = 0; i < 40000000; i++)
	{
		sum2++;
	}
	cout << "��ʱ:������):" << tTime.getElapsedTimeInMilliSec() << endl;
	cout << sum2 << endl;
}

//�Խ���:ֻҪ�������г���{} �Զ�����
void test7() {

	{
		for (int i = 0; i < 2; i++)
		{
			lock_guard<mutex> lg(m);
			sum++;
		}//�뿪����������lock_guard

		cout << "1111" << endl;
	}
}




int main() {
	//test1();
	//test2();
	//test3();
	//test4();
	//test5();
	//test6();
	//test7();

}

#endif