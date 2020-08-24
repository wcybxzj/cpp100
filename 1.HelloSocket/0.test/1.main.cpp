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

class Foo
{
public:
    void foo(int a)
    {
        cout << a << endl;
    }

    static void bar(int a)
    {
        cout << a << endl;
    }
};

void test1() {
    //version 1      
    //void (*pFunc)(int) = &Foo::foo;//error 
    //Foo::foo 的形式参数有两个：一个隐式，一个int
    //void (*pFunc)(int)的形式参数只有一个
    // pFunc(123);

    //修正1-->为其加一个参数即可    
    void (Foo:: * pFunc2)(int) = &Foo::foo;
    Foo f;
    (f.*pFunc2)(12345);

    Foo* pf = &f;
    (pf->*pFunc2)(123124);
    return;
}

void test2() {
    void (*pFunc)(int) = &Foo::bar;//ok
    pFunc(123);
    //这里表明:static成员函数的参数没有隐式参数（本对象）
}

void test3() {
    Foo f;
    //void (Foo::*)(int)->void (*)(Foo*,int)
    (mem_fun(&Foo::foo))(&f, 123);
}

int main(int argc, const char* argv[])
{
    //test1();
    //test2();
    test3();
}


#endif