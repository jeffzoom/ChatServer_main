/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-16 22:11:05
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 13:04:39
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/callee/friendservice.cc
 * @Descripttion: 
 */
#include <iostream>
#include <string>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include <vector>
#include "logger.h"

class FriendService : public fixbug::FiendServiceRpc {
public:
    // 某些本地业务
    std::vector<std::string> GetFriendsList(uint32_t userid) {
        std::cout << "do GetFriendsList service! userid:" << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("gao yang");
        vec.push_back("liu hong");
        vec.push_back("wang shuo");
        return vec;
    }

    // 重写基类方法
    void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendsListRequest* request,
                       ::fixbug::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        // 1.取出数据
        uint32_t userid = request->userid();
        // 2.执行本地业务
        std::vector<std::string> friendsList = GetFriendsList(userid);
        // 3.给response填写响应值
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (std::string &name : friendsList) {
            std::string *p = response->add_friends();
            *p = name;
        }
        // 4.执行done的回调，运行SendRpcResponse
        done->Run();
    }
};

int main(int argc, char **argv) {
    LOG_ERR("ddddd");
    LOG_INFO("ddddd");
    // LOG_INFO("forst log message!");
    // LOG_ERR("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);
    
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象。把FriendService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启动一个rpc服务发布节点   Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}