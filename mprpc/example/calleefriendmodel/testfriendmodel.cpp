/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-22 22:17:08
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 22:23:51
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/callee/testfriendmodel2.cpp
 * @Descripttion: 
 */
#include <iostream>
#include <string>
#include <vector>
#include "friendmodel.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"
#include "friendmodel.hpp" // 假设这个文件中有FriendModel类

class FriendServiceImpl : public friendservice::FriendService {
public:
    // 实现AddFriend方法
    void AddFriend(::google::protobuf::RpcController* controller,
                   const ::friendservice::AddFriendRequest* request,
                   ::friendservice::AddFriendResponse* response,
                   ::google::protobuf::Closure* done) override {
        int userid = request->userid();
        int friendid = request->friendid();
        
        FriendModel model;
        model.insert(userid, friendid);
        
        response->set_success(true);
        response->set_message("Friend added successfully.");
        done->Run();
    }

    // 实现GetFriendsList方法
    void GetFriendsList(::google::protobuf::RpcController* controller,
                        const ::friendservice::GetFriendsListRequest* request,
                        ::friendservice::GetFriendsListResponse* response,
                        ::google::protobuf::Closure* done) override {
        int userid = request->userid();
        
        FriendModel model;
        std::vector<User> friends = model.query(userid);
        
        for (auto& user : friends) {
            auto* friend_info = response->add_friends();
            friend_info->set_id(user.getId());
            friend_info->set_name(user.getName());
            friend_info->set_state(user.getState());
        }
        done->Run();
    }
};

int main(int argc, char** argv) {
    // 初始化RPC框架
    MprpcApplication::Init(argc, argv);

    // 创建并注册FriendService服务
    RpcProvider provider;
    provider.NotifyService(new FriendServiceImpl());

    // 启动RPC服务发布节点
    provider.Run();

    return 0;
}
