/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2023-10-10 21:15:07
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-10-01 16:13:33
 * @FilePath: /Linux_nc/ChatServer/ChatServer_main/src/server/chatservice.cpp
 * @Descripttion: 
 */

#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <vector>

#include "mprpcapplication.h"
#include "friendmodel.pb.h"
#include "groupmodel.pb.h"
// #include "usermodel.pb.h"
// #include "offlinemessagemodel.pb.h"

using namespace std;
using namespace muduo;

// 获取单例对象，instance()是一个接口函数，目的是获取单例对象
ChatService *ChatService::instance() {      
    static ChatService service;             
    return &service;                    
}

// 注册消息以及对应的Handler回调操作  绑定和回调，这是oop面向对象很重要的环节
ChatService::ChatService() {
    
    // 用户基本业务管理相关事件处理回调注册
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    // 群组业务管理相关事件处理回调注册
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    // 连接redis服务器
    if (_redis.connect()) {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid) {
    // 记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    
    if (it == _msgHandlerMap.end()) { 
        // 如果msgid没有对应的事件处理回调，则返回一个默认的处理器，空操作    
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp) {
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    } else {
        return _msgHandlerMap[msgid];
    }
}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do login service!!!";

    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id); // 根据用户名id查数据库
    if (user.getId() == id && user.getPwd() == pwd) { 
        if (user.getState() == "online") {
            // 该用户已经登录，不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another!";
            conn->send(response.dump());
            
        } else {

            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // id用户登录成功后，向redis订阅channel(id)  订阅通道
            _redis.subscribe(id); 
            
            // 数据库的并发操作是由mysql保证的，这个我不用管
            // 登录成功，更新用户状态信息 state offline=>online
            user.setState("online"); // 要集群，看用户是否在线，所以需要是否在线这个状态
            _userModel.updateState(user);

            // 登录成功，记录用户连接信息
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            
            // 数据库查询该用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty()) {
                response["offlinemsg"] = vec;
                // 读取该用户的离线消息后，把该用户的所有离线消息删除掉
                _offlineMsgModel.remove(id);
            }

            // 数据库查询该用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty()) {
                vector<string> vec2;
                for (User &user : userVec) {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }
            
            // 数据库查询用户的群组信息
            vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }
                response["groups"] = groupV;
            }
            conn->send(response.dump());
        }     
    } else {
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "id or password is invalid!";
        conn->send(response.dump());
    }
}

// 处理注册业务 name password
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do reg service!!!";
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user); // 数据库新增一条用户信息
    
    if (state) {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump()); // 发送数据给客户端
    } else {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int toid = js["toid"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end()) {
            // toid在线，转发消息   服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;
        }
    }

    // 查询toid是否在线 
    User user = _userModel.query(toid);
    if (user.getState() == "online") { // b在线，说明a和b没有在一台主机上
    
        _redis.publish(toid, js.dump()); // 把所要发布的消息直接发到redis上的以b用户id命名的通道上，原来以b用户id注册过的通道上就有事件发生了，然后就会给业务层上报
        return;
    }

    // toid不在线，存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time) {

    // int userid = js["id"].get<int>();
    // int friendid = js["friendid"].get<int>();
    // // 存储好友信息
    // _friendModel.insert(userid, friendid); // 操作数据库，添加好友
    // // 没有返回消息

    // 创建RPC Stub
    friendservice::FriendService_Stub stub(new MprpcChannel());

    friendservice::AddFriendRequest add_friend_request;
    friendservice::AddFriendResponse add_friend_response;
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    add_friend_request.set_userid(userid);
    add_friend_request.set_friendid(friendid);
    
    MprpcController add_friend_controller;
    stub.AddFriend(&add_friend_controller, &add_friend_request, &add_friend_response, nullptr);
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group)) { // 操作数据库allgroup，创建群组
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator"); // 操作数据库groupuser，储存群组创建人信息
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    // int userid = js["id"].get<int>();
    // int groupid = js["groupid"].get<int>();
    // _groupModel.addGroup(userid, groupid, "normal");
    // // 要是想客户端接收服务器的响应
    // // 服务器响应参考 ChatService::reg() 的注册成功和注册失败

    // 创建RPC Stub
    groupservice::GroupService_Stub stub(new MprpcChannel());

    groupservice::AddGroupRequest request;
    groupservice::AddGroupResponse response;
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    request.set_userid(userid);
    request.set_groupid(groupid);
    request.set_role("normal");

    MprpcController controller;
    stub.AddGroup(&controller, &request, &response, nullptr);
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex); // 加锁
    for (int id : useridVec) {

        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end()) { // 查看是否在当前主机在线
            // 转发群消息
            it->second->send(js.dump());
        } else {
            // 查询toid是否在线 
            User user = _userModel.query(id);
            if (user.getState() == "online") { // 在其他主机在线
                _redis.publish(id, js.dump()); // 用你的id直接作为通道号，在你这个通道上发布一条消息  我只管发布消息，你所在的主机就能收到订阅的消息
                // 以对端用户的id作为通道号发布一个消息
            } else {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex); // 加锁，大括号是作用域
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end()) {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid); 

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}

// 处理客户端异常退出
// TcpConnectionPtrs是一个只能指针
void ChatService::clientCloseException(const TcpConnectionPtr &conn) {
    User user;
    {
        lock_guard<mutex> lock(_connMutex); // 操作map，要注意线程安全
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it) {
            if (it->second == conn) {
                // 从map表删除用户的链接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getId()); 

    // 更新用户的状态信息
    if (user.getId() != -1) {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

// 服务器异常，业务重置方法
void ChatService::reset() {
    // 把online状态的用户，设置成offline
    _userModel.resetState();
}

// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg) {
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end()) {
        it->second->send(msg); // 如果要发送的目标userid在线，就调用muduo网络库发送消息，若离线，就将消息储存到数据库的离线表中
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}
