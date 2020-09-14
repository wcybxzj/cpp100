#if 1
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;

void worker_thread()
{
   std::cout << "worker_thread()begin()" << std::endl;

   for (int i = 0; i < 3; i++)
   {
       std::cout << "worker thread sleep 1" << std::endl;
       std::chrono::seconds t(1);
       std::this_thread::sleep_for(t);
   }

    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [] {
        std::cout << "worker_thread()->wait()" << std::endl;
        return ready; 
        });

    // after the wait, we own the lock.
    std::cout << "Worker thread is processing data\n";
    data += " after processing";

    // Send data back to main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}

int main()
{
    std::thread worker(worker_thread);

    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_one();

    // wait for the worker
    {
        std::cout << "unique_lock begin()" << std::endl;
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {
            std::cout << "main thread wait()" << std::endl;
            return processed; 
        });
        std::cout << "unique_lock end()" << std::endl;
    }
    std::cout << "Back in main(), data = " << data << '\n';

    worker.join();
}
#endif