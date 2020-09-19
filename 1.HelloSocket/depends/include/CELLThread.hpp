#ifndef CELLTHREAD
#define CELLTHREAD

#include "CELLSemaphore.hpp"
#include "CELL.hpp"

class CELLThread {
private:
	typedef std::function<void(CELLThread*)> EventCall;

	EventCall _onCreate;
	EventCall _onRun;
	EventCall _onDestory;

	std::mutex _mutex;

	CELLSemaphore _sem;

	bool _isRun = false;

public:
	static void Sleep(time_t dt)
	{
		std::chrono::milliseconds t(dt);
		std::this_thread::sleep_for(t);
	}

	void Start(
		EventCall onCreate = nullptr,
		EventCall onRun = nullptr,
		EventCall onDestory = nullptr)
	{
		std::lock_guard<std::mutex>lock(_mutex);
		if (!_isRun)
		{
			_isRun = true;
			if (onCreate)
			{
				_onCreate = onCreate;
			}

			if (onRun)
			{
				_onRun = onRun;
			}

			if (onDestory)
			{
				_onDestory = onDestory;
			}
			std::thread t(std::mem_fn(&CELLThread::OnWork),this);
			t.detach();
		}
	}

	//在其他线程调用结束
	void Close() {
		std::lock_guard<std::mutex> lock(_mutex);
		if (_isRun)
		{
			_isRun = false;
			_sem.wait();
		}
	}

	//在当前线程调用
	void Exit()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_isRun)
		{
			_isRun = false;
		}
	}

	bool isRun()
	{
		return _isRun;
	}

protected:
	void OnWork() {
		if (_onCreate)
		{
			_onCreate(this);
		}

		if (_onRun)
		{
			_onRun(this);
		}

		if (_onDestory)
		{
			_onDestory(this);
		}
		_sem.wakeup();
	}



};


#endif