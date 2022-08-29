#include "chatserver.hpp"
#include "chatservice.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h> //处理服务器异常退出
using namespace std;

//处理服务器ctrl+c结束后，重置重置用户的状态信息为下线
void resetHandler(int) //信号处理函数，参数必须是int类型
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    //解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, resetHandler); //ctrl+c服务器后，重置用户的状态信息为下线

    EventLoop loop;   //epoll
    InetAddress addr(ip, port); //回送地址
    ChatServer server(&loop, addr, "ChatServer");

    server.start(); //启动服务器：将listenfd通过epoll_ctl添加到epfd上
    loop.loop();    //epoll_wait以阻塞方式等待新用户连接、已连接用户的读写事件等
    return 0;
}