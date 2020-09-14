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

using namespace std;

//lock_guard和unquik_lock的区别
std::mutex _mutex;

void func1(int id)
{
	//效果一样
//	std::lock_guard<std::mutex> lock(_mutex);
	std::unique_lock<std::mutex> lock(_mutex);

	cout << "func1() id"<<id <<", begin()" << endl;
	for (int i = 0; i < 3; i++)
	{
		cout <<"id:"<< id <<",i:"<<i<< endl;
		
		std::chrono::seconds t(1);
		std::this_thread::sleep_for(t);
	}
	cout << "func1() id" << id << " end()" << endl;
}

void test1() {
	thread t1(func1, 1);
	thread t2(func1, 2);
	
	t1.detach();
	t2.detach();
}

int main()
{
	test1();
	system("pause");
	return EXIT_SUCCESS;
}
#endif