/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2023-10-10 15:42:03
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-31 20:59:27
 * @FilePath: /Linux_nc/ChatServer/myChatServer/include/server/chatserver.hpp
 * @Descripttion: 
 */
#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

// 网络模块代码
// 聊天服务器的主类
class ChatServer {
public:
    // 初始化聊天服务器对象
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);

    // 启动服务
    void start();

private:
    // 上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &);

    // 上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr &,
                   Buffer *,
                   Timestamp);

    TcpServer _server; 
    EventLoop *_loop;  
};

#endif