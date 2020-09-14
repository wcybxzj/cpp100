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

/*
//https://www.ibm.com/developerworks/cn/aix/library/1307_lisl_c11/index.html
标准库函数 std::move
既然编译器只对右值引用才能调用转移构造函数和转移赋值函数，而所有命名对象都只能是左值引用，
如果已知一个命名对象不再被使用而想对它调用转移构造函数和转移赋值函数，也就是把一个左值引用当做右值引用来使用，怎么做呢？
标准库提供了函数 std::move，这个函数以非常简单的方式将左值引用转换为右值引用。
*/


void ProcessValue(int& i) {
    std::cout << "LValue processed: " << i << std::endl;
}

void ProcessValue(int&& i) {
    std::cout << "RValue processed: " << i << std::endl;
}

//将左值硬转成右值
//LValue processed : 0
//RValue processed : 0
void test1() {
    int a = 0;
    ProcessValue(a);
    ProcessValue(std::move(a));
}

//std::move在提高 swap 函数的的性能上非常有帮助，一般来说，swap函数的通用定义如下：
template <class T> 
void swap1(T& a, T& b)
{
    T tmp(a);   // copy a to tmp 
    a = b;      // copy b to a 
    b = tmp;    // copy tmp to b 
}


//有了 std::move，swap 函数的定义变为 :
//通过 std::move，一个简单的 swap 函数就避免了 3 次不必要的拷贝操作。
template <class T> 
void swap2(T& a, T& b)
{
    T tmp(std::move(a)); // move a to tmp 
    a = std::move(b);    // move b to a 
    b = std::move(tmp);  // move tmp to b 
}


void test2()
{
    int a = 1;
    int b = 2;

    swap2(a, b);

    cout << a << endl;

    cout << b << endl;

}
int main() {
    //test1();
    test2();
}


#endif