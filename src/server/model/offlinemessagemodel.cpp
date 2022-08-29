#include "offlinemessagemodel.hpp"
#include "db.h"

//offlinemessage表相关的操作

//存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg)
{
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", userid, msg.c_str());

    //2.连接数据库，执行sql语句
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

//删除用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", userid);

    //2.连接数据库，执行sql语句
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

//根据userid查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid=%d", userid);

    //2.连接数据库，执行sql语句，返回查询到的结果
    MySQL mysql;
    vector<string> vec;
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            //把userid用户的所有离线消息放入vec中返回
            MYSQL_ROW row;         
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]);
            }
            //释放资源
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}