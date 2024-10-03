/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-06 15:48:48
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 09:59:56
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/callee/userservice.cc
 * @Descripttion: 
 */
#include <iostream>
#include <string>
#include "user.pb.h"            // 发布的函数对应的头文件
#include "mprpcapplication.h"   // rpc框架头文件
#include "rpcprovider.h"        // rpc框架头文件
// #include "../user.pb.h"
// #include "../src/include/mprpcapplication.h"
// #include "../src/include/rpcprovider.h"

/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
*/
// 这一段代码是我在用框架发布服务
class UserService : public fixbug::UserServiceRpc { // 使用在rpc服务发布端（rpc服务提供者）
public:
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name:" << name << " pwd:" << pwd << std::endl;  
        return false;
    }

    // 我有一个本地服务，我想把这个发成远程服务
    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id:" << id << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        // 1框架给业务上报了请求参数LoginRequest，应用获取相应数据做本地业务  从请求里面拿数据
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 2做本地业务
        bool login_result = Login(name, pwd); 

        // 3把响应写入  包括错误码、错误消息、返回值
        // 填写网络消息，发送给RPC框架  填响应消息
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        // code->set_errcode(1);
        // code->set_errmsg("Login do error!");
        response->set_sucess(login_result);

        // 4执行回调操作   执行响应对象数据的序列化和网络发送（都是由框架来完成的）
        done->Run(); // 这个回调就会调用绑定的方法 &RpcProvider::SendRpcResponse, 这个方法用于序列化rpc的响应和网络发送
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done)
    {
        // 1.直接取数据
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 2.做本地业务
        bool ret = Register(id, name, pwd);

        // 3.给response填写响应值
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_sucess(ret);

        // 4.执行done的回调，对于这个回调，框架做的事情就是把填好的response进行数据的序列化并通过网络发回给rpcclient
        done->Run();
    }

};

int main(int argc, char **argv) {
    std::cout << "1111" << std::endl;

    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象。把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService()); // NotifyService是发布rpc服务的站点，并记录服务对象UserService和支持的服务方法Login
    // provider.NotifyService(new ProductService());

    // 启动一个rpc服务发布节点   Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}
