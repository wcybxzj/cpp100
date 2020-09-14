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

    //��������
    //�Ϳ������캯�����ƣ��м�����Ҫע�⣺
    //1. ��������ֵ���ķ��ű�������ֵ���÷��ţ����� && ����
    //2. ��������ֵ���������ǳ�������Ϊ������Ҫ�޸���ֵ��
    //3. ��������ֵ������Դ���Ӻͱ�Ǳ����޸ġ�������ֵ�����������ͻ��ͷ���Դ��ת�Ƶ��¶������ԴҲ����Ч�ˡ�
    MyString(MyString&& str) {
        std::cout << "Move Constructor is called! source: " << str._data << std::endl;
        _len = str._len;
        _data = str._data;
        str._len = 0;
        str._data = NULL;
    }

    //��������
    MyString& operator=(MyString&& str) {
        std::cout << "Move Assignment is called! source: " << str._data << std::endl;
        if (this != &str) {
            _len = str._len;
            _data = str._data;
            str._len = 0;
            str._data = NULL;
        }
        return *this;
    }


    virtual ~MyString() {
        if (_data) free(_data);
    }
};

/*
Move Assignment is called!source: Hello
Move Constructor is called!source : World
*/

/*
�ɴ˿�������������������ֵ����ֵ������ֵ������ת�ƹ��캯����ת�Ƹ�ֵ����������ʡ����Դ������˳������е�Ч�ʡ�
������ֵ���ú�ת�����壬��������ƺ�ʵ����ʱ��������Ҫ��̬���������Դ���࣬Ӧ�����ת�ƹ��캯����ת�Ƹ�ֵ�����������Ӧ�ó����Ч�ʡ�
*/
void test1()
{
    MyString a;
    a = MyString("Hello");

    std::vector<MyString> vec;
    vec.push_back(MyString("World"));
}

int main() {
    test1();
}


#endif