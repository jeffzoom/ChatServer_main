/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-15 09:52:03
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 09:46:37
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/caller/calluserservice.cc
 * @Descripttion: 
 */
#include <iostream>
#include "mprpcapplication.h"   // rpc框架头文件
#include "mprpcchannel.h"       // rpc框架头文件
#include "user.pb.h"            // 调用的函数对应的头文件

int main(int argc, char **argv) {
    // 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);
    
    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    // 填写你想发送的rpc方法的请求参数 演示调用远程发布的rpc方法Login
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    // 定义rpc方法的响应
    fixbug::LoginResponse response;
    // 用代理对象stub发起rpc方法的调用  同步的rpc调用过程  MprpcChannel::callmethod
    stub.Login(nullptr, &request, &response, nullptr); // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    // Login的底层实现是调用了MprpcChannel的CallMethod方法，把参数传进来

    // 一次rpc调用完成，读调用的结果
    if (0 == response.result().errcode()) {
        std::cout << "rpc login response success:" << response.sucess() << std::endl;
    } else {
        std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;
    }

    // 演示调用远程发布的rpc方法Register
    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("mprpc");
    req.set_pwd("666666");
    fixbug::RegisterResponse rsp;

    // 以同步的方式发起rpc调用请求，等待返回结果
    stub.Register(nullptr, &req, &rsp, nullptr);  // 第一个和第四个参数没用

    // 一次rpc调用完成，读调用的结果
    if (0 == rsp.result().errcode()) {
        std::cout << "rpc register response success:" << rsp.sucess() << std::endl;
    } else {
        std::cout << "rpc register response error : " << rsp.result().errmsg() << std::endl;
    }

    return 0;
}
