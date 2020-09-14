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

class A {
public:
	int a;
	A(int len = 123) {
		a = len;
		cout << "A init len:"<< a << endl;
	}
};

class B {
private:
	A a1;

public:
	B():a1(112233) {
		
	}

	void say()
	{
		cout << "ssss" << endl;
	}
};

//Êä³ö:
//A init
//ssss
int main()
{
	B b1;
	b1.say();

	system("pause");
	return EXIT_SUCCESS;
}
#endif