#ifndef _CELL_HPP_
#define _CELL_HPP_

//#define _CRT_SECURE_NO_WARNINGS
//#define WIN32_LEAN_AND_MEAN
////inet_ntoa����
////�������1:
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
////�������2:
////����c++  -> Ԥ���� -> Ԥ����������
////�������3:
////c/c++  -> ���� -> sdl���ѡ���
////�����������ĺ� ����˵winsock2.h�����ظ�����ĳ���
//#include <iostream>
//#include<Windows.h>
//#include<WinSock2.h>
//#include<stdio.h>
//#include<vector>
//#include <ws2tcpip.h>
//#include <thread>
////����1:����Windows ��̬��
//#pragma comment(lib, "ws2_32.lib")
//using namespace std;
////����2:����->��������->������->����->����������->д��ws2_32.lib

#ifdef _WIN32
#define FD_SETSIZE 10024 //windows select֧�ֳ���64��fd ������WinSock2.hǰ��
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
	//������
#define RECV_BUFF_SIZE 10240 //10KB
#endif // !RECV_BUFF_SIZE


#endif