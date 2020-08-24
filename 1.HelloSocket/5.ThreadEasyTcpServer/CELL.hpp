#ifndef _CELL_HPP_
#define _CELL_HPP_

//#define _CRT_SECURE_NO_WARNINGS
//#define WIN32_LEAN_AND_MEAN
////inet_ntoa报错
////解决方法1:
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
////解决方法2:
////属性c++  -> 预处器 -> 预处理器定义
////解决方法3:
////c/c++  -> 常规 -> sdl检查选择否
////如果不加上面的宏 报错说winsock2.h中有重复定义的常量
//#include <iostream>
//#include<Windows.h>
//#include<WinSock2.h>
//#include<stdio.h>
//#include<vector>
//#include <ws2tcpip.h>
//#include <thread>
////方法1:加载Windows 静态库
//#pragma comment(lib, "ws2_32.lib")
//using namespace std;
////方法2:属性->配置属性->连接器->输入->附加依赖项->写入ws2_32.lib

#ifdef _WIN32
#define FD_SETSIZE 10024 //windows select支持超过64个fd 定义在WinSock2.h前面
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<WinSock2.h>
#include<stdio.h>
#include<vector>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h> //uni std
#include<arpa/inet.h>
#include<string.h>
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#include<stdio.h>
#include<vector>
#include<map>
#include<thread>
#include<mutex>
#include<atomic>
#include<functional>
#include"MessageHeader.hpp"
#include"CELLTimestamp.hpp"

#ifndef RECV_BUFF_SIZE
	//缓冲区
#define RECV_BUFF_SIZE 10240 //10KB
#endif // !RECV_BUFF_SIZE


#endif