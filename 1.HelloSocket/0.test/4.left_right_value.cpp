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
//ʵ��ת�ƹ��캯����ת�Ƹ�ֵ����

class MyString {
private:
    char* _data;
    size_t   _len;
    void _init_data(const char* s) {
        _data = new char[_len + 1];
        memcpy(_data, s, _len);
        _data[_len] = '\0';
    }
public:
    MyString() {
        _data = NULL;
        _len = 0;
    }

    MyString(const char* p) {
        _len = strlen(p);
        _init_data(p);
    }

    MyString(const MyString& str) {
        _len = str._len;
        _init_data(str._data);
        std::cout << "Copy Constructor is called! source: " << str._data << std::endl;
    }

    MyString& operator=(const MyString& str) {
        if (this != &str) {
            _len = str._len;
            _init_data(str._data);
        }
        std::cout << "Copy Assignment is called! source: " << str._data << std::endl;
        return *this;
    }

    virtual ~MyString() {
        if (_data) free(_data);
    }
};

void test1()
{
    MyString a;
    MyString("Hello");
}

/*
�����
Copy Assignment is called!source: Hello
Copy Constructor is called!source : World
*/

/*
��� string ���Ѿ���������������ʾ����Ҫ���� main �����У�ʵ���˵��ÿ������캯���Ĳ����Ϳ�����ֵ�������Ĳ�����
MyString(��Hello��) �� MyString(��World��) ������ʱ����Ҳ������ֵ��
��Ȼ��������ʱ�ģ���������Ȼ�����˿�������Ϳ�����ֵ�������û���������Դ������ͷŵĲ�����
����ܹ�ֱ��ʹ����ʱ�����Ѿ��������Դ�����ܽ�ʡ��Դ�����ܽ�ʡ��Դ������ͷŵ�ʱ�䡣�����Ƕ���ת�������Ŀ�ġ�
*/
void test2()
{
    MyString a;
    a = MyString("Hello");

    std::vector<MyString> vec;
    vec.push_back(MyString("World"));
}




int main() {
    //test1();

    test2();
}


#endif