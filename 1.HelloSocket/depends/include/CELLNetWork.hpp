#ifndef _CELL_NET_WORK_HPP_
#define _CELL_NET_WORK_HPP_

#include "CELL.hpp"

class CELLNetWork
{
private:
	CELLNetWork()
	{
#ifdef _WIN32
		//启动Windows socket 2.x环境
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif

#ifndef _WIN32
		signal(SIGPIPE, SIG_IGN);
#endif

#ifndef WIN32
		sigset_t signal_mask;
		sigemptyset(&signal_mask);
		sigaddset(&signal_mask, SIGPIPE);
		int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
		if (rc != 0) {
			CELLLog::Info("block sigpipe error\n");
		}
#endif 
	}

	~CELLNetWork()
	{
#ifdef _WIN32
		//清除Windows socket环境
		WSACleanup();
#endif
	}


public:
	static void Init() {
		static CELLNetWork obj;
	}

};
#endif