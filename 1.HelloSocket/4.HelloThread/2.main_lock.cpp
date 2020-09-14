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
#include<mutex>//锁
#include<atomic>//原子
#include "CELLTimestamp.hpp"

using namespace std;

mutex m;
int sum = 0;
const int tCount = 4;

//写法1：
//atomic_int sum2 = 0;
//写法2：
atomic<int> sum2 = 0;

//普通互斥锁
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
	cout << "耗时:（毫秒):" << tTime.getElapsedTimeInMilliSec() << endl;
	cout << sum << endl;
}

//自解锁:只要代码运行出了{} 自动解锁
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
	cout << "耗时:（毫秒):" << tTime.getElapsedTimeInMilliSec() << endl;
	cout << sum << endl;
}

//原子变量:
void func6() {
	for (int i = 0; i < 10000000; i++)
	{
		sum2++;
	}
}

/*

耗时:（毫秒):1356.46(多线程+原子int 耗时更少)
40000000
耗时 : （毫秒):1424.43
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
	cout << "耗时:（毫秒):" << tTime.getElapsedTimeInMilliSec() << endl;
	cout << sum2 << endl;


	tTime.update();
	sum2 = 0;
	for (int i = 0; i < 40000000; i++)
	{
		sum2++;
	}
	cout << "耗时:（毫秒):" << tTime.getElapsedTimeInMilliSec() << endl;
	cout << sum2 << endl;
}

//自解锁:只要代码运行出了{} 自动解锁
void test7() {

	{
		for (int i = 0; i < 2; i++)
		{
			lock_guard<mutex> lg(m);
			sum++;
		}//离开这个块就析构lock_guard

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