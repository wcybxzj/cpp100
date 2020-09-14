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
	int* const p = new int;
	*p = 123;
	cout << *p << endl;
	*p = 456;
	cout << *p << endl;

	//p = new int;//eerror
}

void test2()
{
	const int*  p = new int;
	//*p = 123;//errror
}

int main()
{





	system("pause");
	return EXIT_SUCCESS;
}
#endif