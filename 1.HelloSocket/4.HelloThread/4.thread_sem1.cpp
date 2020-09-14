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
#include<thread>

using namespace std;


//演示为什么必须让主线程等待子线程运行结束
//父子线程的heap内存是共享的如果

class A {
public:
	int* Pa;

	A()
	{
		Pa = new int(11);
	}

	void OnRun()
	{
		for (int i = 0; i < 100000; i++)
		{		
			(*Pa) += 1;//error 因外主线程把这里delte了
			cout << (*Pa) << endl;
			std::chrono::milliseconds t(100);
			std::this_thread::sleep_for(t);
		}
	}

	void Start() {
			std::thread t = std::thread(std::mem_fn(&A::OnRun), this);
			t.detach();
	}
};

int main()
{
	A* obj1 = new A;
	obj1->Start();
	delete obj1;

	system("pause");
	return EXIT_SUCCESS;
}
#endif