#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

//offlinemessage表的数据操作

#include <string>
#include <vector>//vector可以用json序列化
using namespace std;

//提供离线消息表的操作接口方法
class OfflineMsgModel
{
public:
    //存储用户的离线消息
    void insert(int userid, string msg);

    //删除用户的离线消息
    void remove(int userid);

    //根据userid查询用户的离线消息
    vector<string> query(int userid);
};

#endif