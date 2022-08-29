#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <unordered_map>
#include <functional>

#include "user.hpp"
#include "group.hpp"

//显示当前登录成功用户的基本信息
void showCurrentUserData();

//接收线程, 接收用户的手动输入
void readTaskHandler(int clientfd);

//获取系统时间（聊天信息需要添加时间信息）
string getCurrentTime();

//主聊天页面程序
void mainMenu(int clientfd);


//命令处理器
void help(int fd = 0, string str = "");
void chat(int, string);
void addfriend(int, string);
void creategroup(int, string);
void addgroup(int, string);
void groupchat(int, string);
void loginout(int, string); //退出登录


#endif