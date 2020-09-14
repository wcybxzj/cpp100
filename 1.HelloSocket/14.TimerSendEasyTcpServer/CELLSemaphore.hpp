#ifndef CELLSEMAPHORE
#define CELLSEMAPHORE
#include"CELL.hpp"

#include<chrono>
#include<thread>
#include<condition_variable>

//v1
//功能使用上没问题
//缺点1:_iswaitExit在多线程的使用中没有锁
//缺点2:while(){}死循环判断消耗太大
class CELLSemaphoreV1
{
private:
	int _iswaitExit = true;

public:	
	void wait() {
		while (_iswaitExit)
		{
			Sleep(1);
		}
	}

	void wakeup() {
		_iswaitExit = false;
	}
};

//V2
//加锁了但是deadlock
class CELLSemaphoreV2
{
private:
	int _iswaitExit = true;
	std::mutex _mutex;
	
public:

	//有bug：
	//wait()结束lock_guard mutex才失效 所以一直不是失效果
	//造成wakeup()获取不了mutex
	void wait() {
		std::lock_guard<std::mutex> lock(_mutex);
		while (_iswaitExit)
		{
			Sleep(1);
		}
	}

	void wakeup() {
		std::lock_guard<std::mutex> lock(_mutex);
		_iswaitExit = false;
	}
};


//这个本版的问题是
//如过先调用thread1->wakeup() 
//thread2->wait() 直接就会直接返回,因为wait(lock) 没用第2个参数去判断
class CELLSemaphorev3
{
private:
	int _iswaitExit = true;
	std::mutex _mutex;
	//阻塞等待-条件变量
	std::condition_variable _cv;

public:
	void wait() {
		//unique_lock和lock_guard基本一样只是多一些其他方法
		std::unique_lock<std::mutex> lock(_mutex);
		_iswaitExit = true;
		_cv.wait(lock);
	}

	void wakeup() {
		std::lock_guard<std::mutex> lock(_mutex);
		if (_iswaitExit)
		{
			_iswaitExit = false;
			_cv.notify_one();
		}
	}
};


class CELLSemaphore
{
public:
	//阻塞当前线程
	void wait()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (--_wait >= 0)
		{
			//说明之前执行过wakeup()
			return;
		}
		else {
			//阻塞等待
			_cv.wait(lock, [this]()->bool {
				return _wakeup > 0;
			});
			--_wakeup;
		}
	}
	//唤醒当前线程
	void wakeup()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (++_wait > 0)
		{
			//说明还没执行过wait
			return;
		}
		else {
			++_wakeup;
			_cv.notify_one();
		}
	}

private:
	//改变数据缓冲区时需要加锁
	std::mutex _mutex;
	//阻塞等待-条件变量
	std::condition_variable _cv;
	//等待计数
	int _wait = 0;
	//唤醒计数
	int _wakeup = 0;
};

#endif