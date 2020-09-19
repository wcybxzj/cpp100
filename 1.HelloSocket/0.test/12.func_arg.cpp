#if 1
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


void func1(int tmp3 = 0)
{
	int* tmp = new int();
	
	int* tmp2 = new int();
	
	*tmp2 = 123;
	cout << *tmp2 << endl;

	*tmp = 456;
	memcpy(tmp2, tmp, sizeof(int));
	cout << *tmp2 << endl;

	memcpy(&tmp3, tmp, sizeof(int));
	cout << tmp3 << endl;

	int tmp4;
	memcpy(&tmp4, tmp, sizeof(int));
	cout << tmp4 << endl;

}


void test1()
{
	func1();
}

void func2()
{
	int tmp = 123;
	int tmp4;
	memcpy(&tmp4, &tmp, sizeof(int));
	cout << tmp4 << endl;
}


void test2()
{
	func2();
}


void func3(int n)
{
	cout << &n << endl;
}

void test3()
{
	int n = 123;
	cout << &n << endl;
	func3(n);
}


int main()
{
	//test1();

	//test2();

	test3();

	system("pause");
	return EXIT_SUCCESS;
}
#endif