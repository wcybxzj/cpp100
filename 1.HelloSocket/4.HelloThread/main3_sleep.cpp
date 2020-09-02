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

class Class1 {
public:
	void OnRun()
	{
		while (1) {
			std::chrono::milliseconds t(1000);
			std::this_thread::sleep_for(t);
			
			cout << "111111" << endl;
		}
	}

	void Start() {
		std::thread t(std::mem_fn(&Class1::OnRun), this);
		t.detach();
	}

};


int main()
{

	Class1 c1;
	c1.Start();


	while (1) {
		Sleep(1);
		cout << "2222" << endl;
	}

	system("pause");
	return EXIT_SUCCESS;
}
#endif