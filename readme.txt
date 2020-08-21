2.EasyTcpxxxx: 功能正常
内网:
开2个client 可以看到server和lcient都会卡死

外网:
1个client 可以看到server和lcient都会卡死


g++ -c -o server.o server.cpp -std=c++11 -pthread
g++ server.o -I./ -pthread -std=c++11 -o server

