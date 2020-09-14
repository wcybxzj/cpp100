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
��׼�⺯�� std::move
��Ȼ������ֻ����ֵ���ò��ܵ���ת�ƹ��캯����ת�Ƹ�ֵ��������������������ֻ������ֵ���ã�
�����֪һ�����������ٱ�ʹ�ö����������ת�ƹ��캯����ת�Ƹ�ֵ������Ҳ���ǰ�һ����ֵ���õ�����ֵ������ʹ�ã���ô���أ�
��׼���ṩ�˺��� std::move����������Էǳ��򵥵ķ�ʽ����ֵ����ת��Ϊ��ֵ���á�
*/


void ProcessValue(int& i) {
    std::cout << "LValue processed: " << i << std::endl;
}

void ProcessValue(int&& i) {
    std::cout << "RValue processed: " << i << std::endl;
}

//����ֵӲת����ֵ
//LValue processed : 0
//RValue processed : 0
void test1() {
    int a = 0;
    ProcessValue(a);
    ProcessValue(std::move(a));
}

//std::move����� swap �����ĵ������Ϸǳ��а�����һ����˵��swap������ͨ�ö������£�
template <class T> 
void swap1(T& a, T& b)
{
    T tmp(a);   // copy a to tmp 
    a = b;      // copy b to a 
    b = tmp;    // copy tmp to b 
}


//���� std::move��swap �����Ķ����Ϊ :
//ͨ�� std::move��һ���򵥵� swap �����ͱ����� 3 �β���Ҫ�Ŀ���������
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