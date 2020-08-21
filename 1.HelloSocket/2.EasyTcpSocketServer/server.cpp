//linux:make -f index.txt
#include"EasyTcpServer.hpp"




int main() {
	EasyTcpServer server;
	server.Bind(nullptr, 4567);
	server.Listen(5);

	EasyTcpServer server2;
	server2.Bind(nullptr, 4568);
	server2.Listen(5);

	while (server.isRun())
	{
		server.OnRun();
		server2.OnRun();

	}
	server.Close();
	

	printf("服务器已经退出\n");
	getchar();
	return 0;
}