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

int funA(int a, int b)
{
	printf("funcA\n");
	return a * b;
}


void test1() {
	std::function<int(int, int)>call = funA;
	int n = call(10, 2);
	cout << n << endl;//20
}

void test2()
{
	std::function<int(int, int)> call;
	int n = 5;
	call = [n /*�ⲿ���������б�*/](/*�����б�*/int a, int b) ->int /*����ֵ����*/
	{
		return n + a + b;
	};

	int f = call(3, 1);
	cout << f << endl;//9
}

int main()
{
	//test1();

	test2();
	system("pause");
	return EXIT_SUCCESS;
}


/* lambda���ʽ  ��������ʽ ��������
[ caputrue ] ( params ) opt -> ret { body; };

[ �ⲿ���������б� ] ( ������ ) ��������� -> ����ֵ���� { ������; };

�����б�lambda���ʽ�Ĳ����б�ϸ������lambda���ʽ�ܹ����ʵ��ⲿ�������Լ���η�����Щ������

1) []�������κα�����

2) [&]�����ⲿ�����������б���������Ϊ�����ں�������ʹ�ã������ò��񣩡�

3) [=]�����ⲿ�����������б���������Ϊ�����ں�������ʹ��(��ֵ����)��

4) [=, &foo]��ֵ�����ⲿ�����������б������������ò���foo������

5) [bar]��ֵ����bar������ͬʱ����������������

6) [this]����ǰ���е�thisָ�룬��lambda���ʽӵ�к͵�ǰ���Ա����ͬ���ķ���Ȩ�ޡ�
����Ѿ�ʹ����&���� = ����Ĭ�Ϻ��д�ѡ�
����this��Ŀ���ǿ�����lamda��ʹ�õ�ǰ��ĳ�Ա�����ͳ�Ա������

////////
1).capture�ǲ����б�

2).params�ǲ�����(ѡ��)

3).opt�Ǻ���ѡ�������mutable,exception,attribute��ѡ�

mutable˵��lambda���ʽ���ڵĴ�������޸ı�����ı��������ҿ��Է��ʱ�����Ķ����non-const������

exception˵��lambda���ʽ�Ƿ��׳��쳣�Լ������쳣��

attribute�����������ԡ�

4).ret�Ƿ���ֵ���͡�(ѡ��)

5).body�Ǻ����塣
*/
#endif