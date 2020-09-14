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

//https://www.ibm.com/developerworks/cn/aix/library/1307_lisl_c11/index.html
/*
精确传递(Perfect Forwarding)
本文采用精确传递表达这个意思。”Perfect Forwarding”也被翻译成完美转发，精准转发等，说的都是一个意思。
精确传递适用于这样的场景：需要将一组参数原封不动的传递给另一个函数。
“原封不动”不仅仅是参数的值不变，在 C++ 中，除了参数值之外，还有一下两组属性：
左值／右值和 const / non - const。 精确传递就是在参数传递过程中，所有这些属性和参数值都不能改变。在泛型函数中，这样的需求非常普遍。
下面举例说明。函数 forward_value 是一个泛型函数，它将一个参数传递给另一个函数 process_value。
forward_value 的定义为：
*/


template <typename T>
void process_value(T val)
{
}

template <typename T> void forward_value(const T& val) {
	cout << "int&" << endl;
	process_value(val);
}

template <typename T> void forward_value(T& val) {
	cout << "const int&" << endl;
	process_value(val);
}

//问题:1个函数为了 满足传入参数的不同要写2个版本好累
void test1()
{
	int a = 0;
	const int& b = 1;
	forward_value(a); // const int& 
	forward_value(b); // int& 
	forward_value(2); // int&
}

//只需要定义一次，接受一个右值引用的参数，就能够将所有的参数类型原封不动的传递给目标函数。
//四种不用类型参数的调用都能满足，参数的左右值属性和 const/non-cosnt 属性完全传递给目标函数 process_value。
//这个解决方案不是简洁优雅吗？
template <typename T> void forward_value2(T&& val) {
	cout << "forward_value2()" << endl;
	process_value(val);
}

//解决问题
void test2()
{
	int a = 0;
	const int& b = 1;
	forward_value2(a); // const int& 
	forward_value2(b); // int& 
	forward_value2(2); // int&
}

int main()
{
	//test1();
	test2();
	system("pause");
	return EXIT_SUCCESS;
}
#endif