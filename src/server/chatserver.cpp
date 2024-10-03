/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2023-10-10 15:44:51
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-10-01 16:08:55
 * @FilePath: /Linux_nc/ChatServer/ChatServer_main/src/server/chatserver.cpp
 * @Descripttion: 
 */
#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"

#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace placeholders;
using json = nlohmann::json; // 起一个更短的名称

// 网络模块代码
// 初始化聊天服务器对象
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量 一个主线程，I/O线程，main reactor，用于监听新用户的连接，三个工作线程，处理已连接用户的读写事件
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start() {
    _server.start();
}

// 上报连接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn) {
    // 客户端断开链接
    cout << " onConnection_zsq " << endl;
    
    if (!conn->connected()) { // connected() return true if success.
        ChatService::instance()->clientCloseException(conn); // 可能客户端异常断开，要处理异常
        conn->shutdown();
    }
}

// 上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time) 
{
    string buf = buffer->retrieveAllAsString(); // 从缓冲区读取消息
    cout << buf << " onMessage_zsq " << endl; // 测试，添加json打印代码

    // 数据的反序列化 数据解码
    json js = json::parse(buf);
    
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    
    // 回调消息绑定好的事件处理器，来执行相应的业务处理，实际的运行处理函数，比如说运行login，loginout，createGroup
    msgHandler(conn, js, time);
}
