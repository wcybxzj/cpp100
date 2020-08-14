#define WIN32_LEAN_AND_MEAN

//如果不加上面的宏 报错说winsock2.h中有重复定义的常量
#include<Windows.h>
#include<WinSock2.h>
#include<stdio.h>
//方法1:加载Windows 静态库
//#pragma comment(lib, "ws2_32.lib")


//方法2:属性->配置属性->连接器->输入->附加依赖项->写入ws2_32.lib



//windows socket写法
void test1() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//析构windows socket
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
	//启动windows socket

	//test1();
	
	test2();


	getchar();
	return 0;
}