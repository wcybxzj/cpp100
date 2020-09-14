#if 0
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <condition_variable>
#include <thread>
#include <chrono>

using namespace std;

std::condition_variable cv;
std::mutex cv_m; // This mutex is used for three purposes:
                 // 1) to synchronize accesses to i
                 // 2) to synchronize accesses to std::cerr
                 // 3) for the condition variable cv
int i = 0;

void waits(int id)
{
    std::unique_lock<std::mutex> lk(cv_m);
    std::cerr << "Waiting... \n";
    cv.wait(lk, [=] {
        cout << "wait execute" << id << endl;
        return i == 1; 
        });
    std::cerr << "...finished waiting. i == 1\n";
}

void signals()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        std::cerr << "Notifying...\n";
    }
    cv.notify_all();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        std::lock_guard<std::mutex> lk(cv_m);
        i = 1;
        std::cerr << "Notifying again...\n";
    }
    cv.notify_all();
}

/*
Waiting...
wait execute11
Waiting...
wait execute22
Waiting...
wait execute33
Notifying...
wait execute33
wait execute22
wait execute11
Notifying again...
wait execute11
...finished waiting.i == 1
wait execute22
...finished waiting.i == 1
wait execute33
...finished waiting.i == 1
*/
int main()
{
    std::thread t1(waits,11), t2(waits,22), t3(waits,33), t4(signals);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

#endif