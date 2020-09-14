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
��ȷ����(Perfect Forwarding)
���Ĳ��þ�ȷ���ݱ�������˼����Perfect Forwarding��Ҳ�����������ת������׼ת���ȣ�˵�Ķ���һ����˼��
��ȷ���������������ĳ�������Ҫ��һ�����ԭ�ⲻ���Ĵ��ݸ���һ��������
��ԭ�ⲻ�����������ǲ�����ֵ���䣬�� C++ �У����˲���ֵ֮�⣬����һ���������ԣ�
��ֵ����ֵ�� const / non - const�� ��ȷ���ݾ����ڲ������ݹ����У�������Щ���ԺͲ���ֵ�����ܸı䡣�ڷ��ͺ����У�����������ǳ��ձ顣
�������˵�������� forward_value ��һ�����ͺ���������һ���������ݸ���һ������ process_value��
forward_value �Ķ���Ϊ��
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

//����:1������Ϊ�� ���㴫������Ĳ�ͬҪд2���汾����
void test1()
{
	int a = 0;
	const int& b = 1;
	forward_value(a); // const int& 
	forward_value(b); // int& 
	forward_value(2); // int&
}

//ֻ��Ҫ����һ�Σ�����һ����ֵ���õĲ��������ܹ������еĲ�������ԭ�ⲻ���Ĵ��ݸ�Ŀ�꺯����
//���ֲ������Ͳ����ĵ��ö������㣬����������ֵ���Ժ� const/non-cosnt ������ȫ���ݸ�Ŀ�꺯�� process_value��
//�������������Ǽ��������
template <typename T> void forward_value2(T&& val) {
	cout << "forward_value2()" << endl;
	process_value(val);
}

//�������
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