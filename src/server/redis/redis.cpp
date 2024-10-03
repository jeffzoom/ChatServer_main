/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2023-10-22 17:58:05
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-20 19:58:00
 * @FilePath: /Linux_nc/ChatServer/myChatServer/src/server/redis/redis.cpp
 * @Descripttion: 
 */
#include "redis.hpp"
#include <iostream>
using namespace std;

Redis::Redis() : _publish_context(nullptr), _subcribe_context(nullptr) {}

Redis::~Redis() {
    if (_publish_context != nullptr) {redisFree(_publish_context);} // 释放_publish_context，防止内存泄漏
    if (_subcribe_context != nullptr) {redisFree(_subcribe_context);}
}

// 创建两个 Redis 连接，一个用于发布消息，一个用于订阅消息
// 创建一个新线程来调用 observer_channel_message() 函数，该函数用于监听订阅通道上的消息
bool Redis::connect() {
    // 创建了两个连接
    // 负责publish发布消息的上下文连接
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _publish_context) {
        cerr << "connect redis failed!" << endl;
        return false;
    }
    // 负责subscribe订阅消息的上下文连接
    _subcribe_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _subcribe_context) {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    thread t([&]() {
        observer_channel_message();
    });
    t.detach(); // 线程分离，设为守护线程

    cout << "connect redis-server success!" << endl;
    return true;
}

// 向redis指定的通道channel发布消息，channel是通道号，message是消息
bool Redis::publish(int channel, string message) {
    // 发布消息
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if (nullptr == reply) {
        cerr << "publish command failed!" << endl;
        return false;
    }
    freeReplyObject(reply); // reply是动态生成的结构体，用完之后要free释放掉
    return true;
}

// 向redis指定的通道subscribe订阅消息
bool Redis::subscribe(int channel) {
    if (REDIS_ERR == redisAppendCommand(this->_subcribe_context, "SUBSCRIBE %d", channel)) {
        cerr << "subscribe command failed!" << endl;
        return false;
    }
    // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
    int done = 0;
    while (!done) {
        if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done)) {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    // redisGetReply

    return true;
}

// 向redis指定的通道unsubscribe取消订阅消息
bool Redis::unsubscribe(int channel) {
    if (REDIS_ERR == redisAppendCommand(this->_subcribe_context, "UNSUBSCRIBE %d", channel)) {
        cerr << "unsubscribe command failed!" << endl;
        return false;
    }
    // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
    int done = 0;
    while (!done) {
        if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done)) {
            cerr << "unsubscribe command failed!" << endl;
            return false;
        }
    }
    return true;
}

// 在独立线程中接收订阅通道中的消息  
void Redis::observer_channel_message() {
    redisReply *reply = nullptr;
    
    // 循环调用 redisGetReply 从 Redis 服务器获取订阅的消息
    while (REDIS_OK == redisGetReply(this->_subcribe_context, (void **)&reply)) {
        // 订阅收到的消息是一个带三元素的数组
        if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr) {
            // 给业务层上报通道上发生的消息，1是通道号(id)，2是这个通道发生的数据
            _notify_message_handler(atoi(reply->element[1]->str) , reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
    cerr << ">>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<" << endl;
}

// 用于设置消息通知处理函数 _notify_message_handler，该函数由外部提供，用于处理接收到的消息
void Redis::init_notify_handler(function<void(int,string)> fn) {
    this->_notify_message_handler = fn;
}
