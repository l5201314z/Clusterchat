#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;


//redis作为集群服务器通信的发布-订阅消息队列中间件

class Redis
{
public:
    Redis();
    ~Redis();

    //连接redis服务器 
    bool connect();

    //向redis指定的通道channel发布消息
    bool publish(int channel, string message);

    //向redis指定的通道订阅消息
    bool subscribe(int channel);

    //向redis指定的通道取消订阅消息，用户下线时执行
    bool unsubscribe(int channel);

    //在独立线程中接收订阅通道中的消息
    void observer_channel_message();

    //初始化向业务层上报通道消息的回调对象
    void init_notify_handler(function<void(int, string)> fn);

private:
    //hiredis同步上下文对象，负责publish消息，有两个上下文(context)的原因：
    //subscribe是阻塞的，上下文进行subscribe的时候，该上下文就被阻塞住了，只有其他上下文才能进行publish
    redisContext *_publish_context;

    //hiredis同步上下文对象，负责subscribe消息
    redisContext *_subcribe_context;

    //回调操作，收到订阅的消息，给service层上报
    function<void(int, string)> _notify_message_handler;  //int: channel号  string: 发送的消息
};

#endif
