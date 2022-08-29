#include "friendmodel.hpp"
#include "db.h"

//friend表相关的操作

//添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, '%d')", userid, friendid);

    //2.连接数据库，执行sql语句
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

//返回用户好友列表，返回给客户端，好友列表直接存在客户端
vector<User> FriendModel::query(int userid)
{
    //1.组装sql语句
    char sql[1024] = {0};
    //联合friend表和userid表 查userid的好友的详细信息
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d", userid);

    //2.连接数据库，执行sql语句，返回查询到的结果
    MySQL mysql;
    vector<User> vec;
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            //把userid用户的所有好友信息放入vec中返回
            MYSQL_ROW row;         
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            //释放资源
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}