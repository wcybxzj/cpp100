#define WIN32_LEAN_AND_MEAN

//�����������ĺ� ����˵winsock2.h�����ظ�����ĳ���
#include<Windows.h>
#include<WinSock2.h>
#include<stdio.h>
//����1:����Windows ��̬��
//#pragma comment(lib, "ws2_32.lib")


//����2:����->��������->������->����->����������->д��ws2_32.lib



//windows socketд��
void test1() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//����windows socket
	WSACleanup();
}

void test2() {
	struct A {
		int age;
	};


	struct B {
		struct A header;
		int sex;
	};

	B var;
	var.header.age = 11;
	var.sex = 222;

	printf("age:%d, sex:%d\n", var.header.age, var.sex);


	struct C :public A {
		int sex;
	};

	C var1;
	var1.age = 33;
	var1.sex = 44;

	printf("age:%d, sex:%d\n", var1.age, var1.sex);

}



int main() {
	//����windows socket

	//test1();
	
	test2();


	getchar();
	return 0;
}