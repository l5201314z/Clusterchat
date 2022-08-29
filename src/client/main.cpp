#include "client.hpp"
#include "public.hpp"
#include "json.hpp"
using json = nlohmann::json;

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <thread>
#include <string>
using namespace std;

//记录当前系统登录的用户信息, 这些信息记录在客户端就好，没必要每次去服务器去拉取
User g_currentUser;
//记录当前登录用户的好友列表信息
vector<User> g_currentUserFriendList;
//记录当前登录用户的群组列表信息
vector<Group> g_currentUserGroupList;
//控制聊天室页面，登录进去为真，loginout则置为假
bool isMainMenuRunning = false;

//聊天客户端程序实现，main线程用作发送线程，子线程用作接收线程
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invalid example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    //解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    //创建客户端的socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        cerr << "socket creat error" << endl;
        exit(-1);
    }

    //填写client需要连接的server信息ip+port
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    //client和server进行连接
    if (-1 == connect(clientfd, (sockaddr *)&server, sizeof(sockaddr_in)))
    {
        cerr << "connect server error" << endl;
        close(clientfd);
        exit(-2);
    }

    //main线程：接收用户输入，发送数据
    for (;;)
    {
        //显示首页面菜单 登录 注册 退出
        cout << "=================================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "=================================" << endl;
        cout << "please choose: ";
        int choice = 0;
        cin >> choice;
        cin.get(); //读掉缓冲区残留的回车

        switch (choice)
        {
        case 1: //login
        {
            //组装向服务器发送的json对象
            int id = 0;
            char pwd[50] = {0};
            cout << "userid:";
            cin >> id;
            cin.get(); //读掉缓冲区残留的回车
            cout << "userpassword:";
            cin.getline(pwd, 50); //getline默认回车符结束，它可以读取空格，而cin>>不读取空格

            json js;
            js["msgid"] = LOGIN_MSG;
            js["id"] = id;
            js["password"] = pwd;
            string request = js.dump();

            //向服务器发送
            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (len == -1)
            {
                cerr << "send login error:" << request << endl;
            }
            else
            {
                //接收服务器的反馈, 阻塞等待服务器的响应
                char buffer[1024] = {0};
                len = recv(clientfd, buffer, 1024, 0);
                if (-1 == len) //接收数据失败
                {
                    cerr << "recv login response error" << endl;
                }
                else
                {
                    json responsejs = json::parse(buffer);
                    if (0 != responsejs["errno"].get<int>()) //登录失败
                    {
                        cerr << responsejs["errmsg"] << endl;
                    }
                    else //登录成功
                    {
                        //记录当前用户的id和name
                        g_currentUser.setId(responsejs["id"].get<int>());
                        g_currentUser.setName(responsejs["name"]);

                        //记录当前用户的好友列表信息
                        if (responsejs.contains("friends")) //如果js字符串包含friends..(有些人有好友，有些人没好友)
                        {
                            //初始化
                            g_currentUserFriendList.clear(); //如果用户没关闭客户端，退出登录再登录，又会从服务器拉取用户的好友列表信息，
                            //所以拉取之前先clear，防止重复往全局好友列表里添加内容

                            vector<string> vec = responsejs["friends"];
                            for (string &str : vec)
                            {
                                json js = json::parse(str);
                                User user;
                                user.setId(js["id"].get<int>());
                                user.setName(js["name"]);
                                user.setState(js["state"]);
                                g_currentUserFriendList.push_back(user);
                            }
                        }

                        //记录当前用户的群组列表信息
                        if (responsejs.contains("groups"))
                        {
                            //初始化
                            g_currentUserGroupList.clear();

                            vector<string> vec1 = responsejs["groups"];
                            for (string &groupstr : vec1)
                            {
                                json grpjs = json::parse(groupstr);
                                Group group;
                                group.setId(grpjs["id"].get<int>());
                                group.setName(grpjs["groupname"]);
                                group.setDesc(grpjs["groupdesc"]);

                                vector<string> vec2 = grpjs["users"];
                                for (string &userstr : vec2)
                                {
                                    GroupUser user;
                                    json js = json::parse(userstr);
                                    user.setId(js["id"].get<int>());
                                    user.setName(js["name"]);
                                    user.setState(js["state"]);
                                    user.setRole(js["role"]);
                                    group.getUsers().push_back(user);
                                }
                                g_currentUserGroupList.push_back(group);
                            }
                        }

                        //显示当前登录用户的基本信息
                        showCurrentUserData();

                        //显示当前用户的离线消息，个人聊天消息或者群组消息
                        if (responsejs.contains("offlinemsg"))
                        {
                            vector<string> vec = responsejs["offlinemsg"];
                            for (string &str : vec)
                            {
                                json js = json::parse(str);
                                //time + [id] + name + " said "" + xxx
                                if (ONE_CHAT_MSG == js["msgid"].get<int>())
                                {
                                    cout << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                                         << " said: " << js["msg"].get<string>() << endl;
                                }
                                else
                                {
                                    cout << "群消息[" << js["groupid"] << "]: " << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                                         << " said: " << js["msg"].get<string>() << endl;
                                }
                            }
                        }

                        //登录成功，启动接收线程负责接收数据，该线程只启动一次，用户没关闭客户端，登出再登入，该线程不需要重启，因为线程函数里的recv是阻塞的
                        static int readthreadnum = 0;//保证readTask只在客户端启动时候创建一次，后续用户只要不关闭客户端，登出再登入，用的都是同一个readTask
                        if(readthreadnum == 0)
                        {
                            std::thread readTask(readTaskHandler, clientfd);
                            readTask.detach(); //分离线程，由操作系统释放任何分配的资源。
                            readthreadnum++;
                        }
                        //进入聊天主菜单页面
                        isMainMenuRunning = true;
                        mainMenu(clientfd);
                    }
                }
            }
        }
        break;
        case 2: //register
        {
            //组装向服务器发送的json对象
            char name[50] = {0};
            char pwd[50] = {0};
            cout << "username:";
            cin.getline(name, 50);
            cout << "userpassword:";
            cin.getline(pwd, 50);

            json js;
            js["msgid"] = REG_MSG;
            js["name"] = name;
            js["password"] = pwd;
            string request = js.dump();

            //向服务器发送
            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (len == -1)
            {
                cerr << "send reg msg error:" << request << endl;
            }
            else
            {
                //接收服务器的反馈, 阻塞等待服务器的响应
                char buffer[1024] = {0};
                len = recv(clientfd, buffer, 1024, 0);
                if (-1 == len)
                {
                    cerr << "recv reg response error " << endl;
                }
                else
                {
                    json responsejs = json::parse(buffer);
                    if (0 != responsejs["errno"].get<int>()) //注册失败，注册成功errno是0
                    {
                        cerr << name << "is already exist, register error!" << endl;
                    }
                    else //注册成功
                    {
                        cerr << name << "register success, userid is " << responsejs["id"]
                             << ", please do not forget it!" << endl;
                    }
                }
            }
        }
        break;
        case 3: //quit
            close(clientfd);
            exit(0);
        default:
            cerr << "输入不合法！" << endl;
            break;
        }
    }

    return 0;
}
