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
#include<chrono>
using namespace std;

class Class1 {
public:
	~Class1() {
		printf("destuct Class1\n");
	}

	void func1() {
		while (true)
		{
			printf("func1()\n");
		}
	}

	void func2() {
		while (true)
		{
			printf("func2()\n");
		}
	}

};

Class1 *c1;

void f1() {
	c1->func1();
}

void f2() {
	c1->func1();
}

void f3() {
	delete c1;
	for (int i = 0; i < 1000; i++)
	{
		printf("delete c1\n");
	}
}

void test1() {
	c1 = new Class1();
	delete c1;
	c1->func1();//still work
}


void test2() {
	c1 = new Class1();

	std::thread t1(f1);
	t1.detach();

	std::thread t2(f2);
	t2.detach();

	std::chrono::seconds t(1);
	std::this_thread::sleep_for(t);

	std::thread t3(f3);
	t3.detach();
}

int main()
{


	system("pause");
	return EXIT_SUCCESS;
}
#endif