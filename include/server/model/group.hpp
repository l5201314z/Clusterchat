#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"
#include <string>
#include <vector>
using namespace std;

//Group表的ORM映射类

//群组相关的业务：群主创建群，组员加入群，群聊
//AllGroup表：id，groupname，groupdesc
//GroupUser表：groupid，userid，grouprole
//群组和组员是多对多的关系，所以有GroupUser表来体现它们之间的关系

class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setDesc(string desc) { this->desc = desc; }

    int getId() { return this->id; }
    string getName() { return this->name; }
    string getDesc() { return this->desc; }
    vector<GroupUser> &getUsers() { return this->users; }

protected:
    int id;
    string name;
    string desc;
    vector<GroupUser> users; //存储组员
};

#endif