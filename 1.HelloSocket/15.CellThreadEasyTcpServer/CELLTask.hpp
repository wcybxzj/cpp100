#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_

#include<thread>
#include<mutex>
#include<list>
#include<functional>

#include "CELLSemaphore.hpp"
#include "CELLThread.hpp"

class CellTaskServer 
{
public:
	int serverId = -1;

private:
	//使用lamda做匿名函数做为每一个任务，之前需要接口+对象传入的对象指针
	//类型名是CellTask 类型值是void()
	typedef std::function<void()> CellTask;

	std::list<CellTask> _tasks;
	std::list<CellTask> _tasksBuf;
	std::mutex _mutex;
	CELLThread _thread;

public:
	void addTask(CellTask task)
	{
		std::lock_guard<std::mutex>lock(_mutex);
		_tasksBuf.push_back(task);
	}

	void Start()
	{
		_thread.Start(
			nullptr,
			[this](CELLThread* pThread) {
				OnRun(pThread);
			}
		);
	}

	void Close()
	{
		printf("CellTaskServer->Close()  cellserverid:%d begin\n", serverId);
		_thread.Close();
		printf("CellTaskServer->Close()  cellserverid:%d end\n", serverId);
	}

protected:
	void OnRun(CELLThread* pThread)
	{
		while (pThread->isRun())
		{
			if (!_tasksBuf.empty())
			{
				std::lock_guard<std::mutex>lock(_mutex);

				for (auto pTask:_tasksBuf)
				{
					_tasks.push_back(pTask);
				}
				_tasksBuf.clear();
			}

			if (_tasks.empty())
			{
				std::chrono::milliseconds t(100);
				std::this_thread::sleep_for(t);
				continue;
			}

			for (auto pTask:_tasks)
			{
				pTask();
			}
			_tasks.clear();
		}

		printf("CellTaskServer%d.OnRun exit\n", serverId);
	}
};

#endif