客户端，注册需要输入用户名和密码，登录需要id和密码来登录

注册:     {"msgid:3","name":"wang","password":"111111"}

登录:     {"msgid":1,"id":1,"password":"123456"}

聊天消息: {"msgid":5, "id":2,"from":"li si","to":1,"msg":"hello1"}  wang给id为2的用户发消息

回复消息: {"msgid":5, "id":2,"from":"li si","to":1,"msg":"ok"}   msg是中文json解析会出错

添加好友: {"msgid":6, "id":1, "friendid":2}