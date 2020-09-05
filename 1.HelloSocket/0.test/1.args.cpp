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
//变参测试:

template <typename T>
void printValue(const T& t)
{
	cout << t << ",";//最后一个元素
}

template <typename T, typename... Args>
void printValue(const T& t, const Args&... args)
{
	cout << t << ",";
	printValue(args...);
}

int main()
{
	printValue("hello", "wolrd", "!");
	system("pause");
}

#endif