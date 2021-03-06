#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_

#include<thread>
#include<mutex>
#include<list>
#include<functional>
#include "CELL.hpp"

class CellTask
{
public:
	CellTask()
	{
	}

	virtual ~CellTask()
	{

	}

	virtual void doTask()
	{

	}

private:

};

typedef std::shared_ptr<CellTask> CellTaskPtr;

class CellTaskServer 
{
private:
	std::list<CellTaskPtr> _tasks;
	std::list<CellTaskPtr> _tasksBuf;
	std::mutex _mutex;

public:
	void addTask(CellTaskPtr task)
	{
		std::lock_guard<std::mutex>lock(_mutex);
		_tasksBuf.push_back(task);
	}

	void Start()
	{
		std::thread t(std::mem_fn(&CellTaskServer::OnRun), this);
		t.detach();
	}

protected:
	void OnRun()
	{
		while (true)
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
				pTask->doTask();
			}
			_tasks.clear();
		}




	}
};

#endif