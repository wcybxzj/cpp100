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

using namespace std::chrono;


void test1()
{
    auto t1 = std::chrono::high_resolution_clock::now();

    auto t2 = std::chrono::high_resolution_clock::now();

    // floating-point duration: no duration_cast needed
    std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

    // integral duration: requires duration_cast
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    // converting integral duration to integral duration of shorter divisible time unit:
    // no duration_cast needed
    std::chrono::duration<long, std::micro> int_usec = int_ms;

    std::cout << "f() took " << fp_ms.count() << " ms, "
        << "or " << int_ms.count() << " whole milliseconds "
        << "(which is " << int_usec.count() << " whole microseconds)" << std::endl;


}

void test2()
{
    //0 1 2 3 4
    for (int i = 0; i < 10; i++)
    {
        if (i == 5)
        {
            break;
        }
        std::cout << i<< " ";
    }
    std::cout <<"============="<< std::endl;

    //0 1 2 3 4
    for (int i = 0; i < 10;)
    {
        if (i == 5)
        {
            break;
        }
        std::cout << i << " ";
        i++;
    }

    std::cout << "=============" << std::endl;

    //0 1 2 3 4 6 7 8 9
    for (int i = 0; i < 10; i++)
    {
        if (i==5)
        {
            continue;
        }
        std::cout << i << " ";
    }

    std::cout << "=============" << std::endl;

    //0 1 2 3 4
    for (int i = 0; i < 10; )
    {
        if (i == 5)
        {
            printf("ËÀÑ­»·\n");
            continue;
        }
        std::cout << i << " ";
        i++;
    }

}

int main()
{
    test2();
}


#endif