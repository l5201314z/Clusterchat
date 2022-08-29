# 项目概述

使用C++编程语言基于muduo网络库实现的简易聊天服务器，实现了用户注册、登录，添加好友， 创建群组， 一对一聊天，群聊， 接收离线消息等功能 。使用 json 作为数据序列化和反序列化工具。  

- nginx配置tcp负载均衡  
- 基于发布-订阅的服务器中间件消息队列 redis 解决集群环境中跨服务器聊天通信  
- MySQL 数据库编程
- Cmake 构建项目编译环境  

# 运行项目

**项目环境**

- Ubuntu 18.04

- Cmake 3.10.2

- boost + muduo

- Redis server v=4.0.9

- MySQL 14.14，需要在MySQL中创建chat数据库，创建5张表，具体操作：[数据库环境](#数据库环境)

- nginx 1.14.0，配置nginx负载均衡，路径`/usr/local/nginx/conf/nginx.conf`，在配置文件中添加如下代码：
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/2021022711021123.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2h1aWZhZ3VhbmdkZW1hbw==,size_16,color_FFFFFF,t_70)

**编译**

- 执行自动编译脚本：`./autobuild.sh`

**运行**

- 启动两台服务器：`cd bin`，`./ChatServer 127.0.0.1 6000`，`./ChatServer 127.0.0.1 6002`

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210227110237352.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2h1aWZhZ3VhbmdkZW1hbw==,size_16,color_FFFFFF,t_70)


- 启动两个客户端：`./ChatClient 127.0.0.1 8000`，`./ChatClient 127.0.0.1 8000`

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210227110252775.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2h1aWZhZ3VhbmdkZW1hbw==,size_16,color_FFFFFF,t_70)


- 聊天

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210227110305508.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2h1aWZhZ3VhbmdkZW1hbw==,size_16,color_FFFFFF,t_70)


# 数据库环境

运行项目前请确保已安装MySQL环境，MySQL的配置信息在/src/server/db/db.cpp，请将用户名与密码修改成自己创建的用户名与密码，本项目代码中默认配置信息如下：

```cpp
//数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat";
```

配置好之后创建chat数据库，在数据库中创建如下5个表

**创建friend表**

friend表：存用户id和该用户对应的好友的id

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210227110338876.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2h1aWZhZ3VhbmdkZW1hbw==,size_16,color_FFFFFF,t_70)


```sql
create table friend(
	userid INT NOT NULL,
	friendid INT NOT NULL,
	PRIMARY KEY(userid, friendid));
```

**创建allgroup表**

allgroup表：存储群组的基本信息，群主id、群名、群简介

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210227110427504.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2h1aWZhZ3VhbmdkZW1hbw==,size_16,color_FFFFFF,t_70)


```sql
create table allgroup(
	id INT PRIMARY KEY AUTO_INCREMENT,
	groupname VARCHAR(50) NOT NULL UNIQUE,
	groupdesc VARCHAR(200) DEFAULT '');
```

**创建groupuser表**

groupuser表：存一个群的群员信息，群员id、群员角色（群主，普通成员）

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210227110439548.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2h1aWZhZ3VhbmdkZW1hbw==,size_16,color_FFFFFF,t_70)


```sql
create table groupuser(
	groupid int not null,
	userid int not null,
	grouprole enum('creator', 'normal') default 'normal',
	primary key(groupid, userid));
```

**创建offlinemessage表**

offlinemessage表：存储用户的离线消息，id: message

注意：该表里userid不能设置为primary key，因为用户可能收到一个用户的多条离线消息，如果设置为主键那就只能收到对方用户的一条离线消息，如果对方再想发送离线消息，就会插入该表失败

虽然该表没有主键，但是InnoDB存储引擎会给该表添加一个主键

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210227110451157.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2h1aWZhZ3VhbmdkZW1hbw==,size_16,color_FFFFFF,t_70)


```sql
create table offlinemessage(
	userid INT NOT NULL,
	message VARCHAR(500) NOT NULL);
```






