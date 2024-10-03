/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-15 17:22:10
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-21 23:31:55
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/include/mprpcchannel.h
 * @Descripttion: 
 */
#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

class MprpcChannel : public google::protobuf::RpcChannel {
public:
    // 重写方法
    // 所有通过stub代理对象调用的rpc方法，都走到这里了，统一做rpc方法调用的数据数据序列化和网络发送 
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, 
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response,
                          google::protobuf:: Closure* done);
};
