#ifndef CELLSEMAPHORE
#define CELLSEMAPHORE
#include"CELL.hpp"

#include<chrono>
#include<thread>
#include<condition_variable>

//v1
//����ʹ����û����
//ȱ��1:_iswaitExit�ڶ��̵߳�ʹ����û����
//ȱ��2:while(){}��ѭ���ж�����̫��
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
//�����˵���deadlock
class CELLSemaphoreV2
{
private:
	int _iswaitExit = true;
	std::mutex _mutex;
	
public:

	//��bug��
	//wait()����lock_guard mutex��ʧЧ ����һֱ����ʧЧ��
	//���wakeup()��ȡ����mutex
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


//��������������
//����ȵ���thread1->wakeup() 
//thread2->wait() ֱ�Ӿͻ�ֱ�ӷ���,��Ϊwait(lock) û�õ�2������ȥ�ж�
class CELLSemaphorev3
{
private:
	int _iswaitExit = true;
	std::mutex _mutex;
	//�����ȴ�-��������
	std::condition_variable _cv;

public:
	void wait() {
		//unique_lock��lock_guard����һ��ֻ�Ƕ�һЩ��������
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
	//������ǰ�߳�
	void wait()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (--_wait >= 0)
		{
			//˵��֮ǰִ�й�wakeup()
			return;
		}
		else {
			//�����ȴ�
			_cv.wait(lock, [this]()->bool {
				return _wakeup > 0;
			});
			--_wakeup;
		}
	}
	//���ѵ�ǰ�߳�
	void wakeup()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (++_wait > 0)
		{
			//˵����ûִ�й�wait
			return;
		}
		else {
			++_wakeup;
			_cv.notify_one();
		}
	}

private:
	//�ı����ݻ�����ʱ��Ҫ����
	std::mutex _mutex;
	//�����ȴ�-��������
	std::condition_variable _cv;
	//�ȴ�����
	int _wait = 0;
	//���Ѽ���
	int _wakeup = 0;
};

#endif