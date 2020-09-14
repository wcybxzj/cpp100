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


void test1()
{
	time_t t;
	t = time(&t);
	//Today'sdateandtime:Mon Sep 14 16:28:25 2020
	printf("Today'sdateandtime:%s\n", ctime(&t));

	std::tm* now = std::gmtime(&t);
	//[2020-9-14 8:29:45]
	printf("[%d-%d-%d %d:%d:%d]",
		now->tm_year + 1900, now->tm_mon + 1,
		now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
}

int main()
{
	test1();


	system("pause");
	return EXIT_SUCCESS;
}
#endif