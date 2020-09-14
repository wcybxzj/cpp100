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

class A {
private:
	A() {
		cout << "A()" << endl;
	}
public:
	static void Init()
	{
		static A obj;
	}

};



int main()
{
	A::Init();
	A::Init();


	system("pause");
	return EXIT_SUCCESS;
}
#endif