#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"

//定义群组用户

//群组用户，相比于user表中的user多了一个role角色，从user类直接继承，复用user的其他信息
class GroupUser : public User
{
public:
    void setRole(string role) { this->role = role; }
    string getRole() { return this->role; }

private:
    string role;
};

#endif