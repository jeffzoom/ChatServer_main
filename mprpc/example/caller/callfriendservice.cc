/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-16 22:11:18
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 13:59:39
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/caller/callfriendservice.cc
 * @Descripttion: 
 */
#include <iostream>
#include "mprpcapplication.h" // 使用mprpc框架只用包含这一个头文件，别的头文件都在mprpcapplication.h中被包含了
#include "friend.pb.h"

int main(int argc, char **argv) {
    // 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::FiendServiceRpc_Stub stub(new MprpcChannel());
    
    // rpc方法的请求参数
    fixbug::GetFriendsListRequest request;
    request.set_userid(1000);
    
    // rpc方法的响应
    fixbug::GetFriendsListResponse response;
    // 发起rpc方法的调用  同步的rpc调用过程  MprpcChannel::callmethod
    MprpcController controller; // 想知道调用过程中的状态信息，因此创建一个controller对象
    // 发起rpc方法的调用，这里实际上会调用到MprpcChannel::CallMethod
    stub.GetFriendsList(&controller, &request, &response, nullptr); // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    
    // 一次rpc调用完成，读调用的结果
    if (controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl; // 如果有问题，这里会打印错误信息
    } else {
        if (0 == response.result().errcode()) {
            std::cout << "rpc GetFriendsList response success!" << std::endl;
            int size = response.friends_size();
            for (int i=0; i < size; ++i) {
                std::cout << "index:" << (i+1) << " name:" << response.friends(i) << std::endl;
            }
        } else {
            std::cout << "rpc GetFriendsList response error : " << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}