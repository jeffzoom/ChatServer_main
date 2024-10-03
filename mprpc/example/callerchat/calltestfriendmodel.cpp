/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-22 22:25:53
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-08-02 18:24:42
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/callerchat/calltestfriendmodel.cpp
 * @Descripttion: 
 */
#include <iostream>
#include "mprpcapplication.h"
#include "friendmodel.pb.h"

int main(int argc, char** argv) {
    // 初始化RPC框架
    MprpcApplication::Init(argc, argv);

    // 创建RPC Stub
    friendservice::FriendService_Stub stub(new MprpcChannel()); // new MprpcChannel()创建了一个MprpcChannel对象，该对象实现了CallMethod函数

    // 调用AddFriend方法
    {
        // friendservice::AddFriendRequest add_friend_request;
        // friendservice::AddFriendResponse add_friend_response;
        // int userid = 25;
        // int friendid = 15;
        // add_friend_request.set_userid(userid);
        // add_friend_request.set_friendid(friendid);
        
        // MprpcController add_friend_controller;
        // stub.AddFriend(&add_friend_controller, &add_friend_request, &add_friend_response, nullptr);
        // if (add_friend_controller.Failed()) {
        //     std::cout << "AddFriend RPC failed: " << add_friend_controller.ErrorText() << std::endl;
        // } else {
        //     std::cout << "AddFriend response: " << add_friend_response.message() << std::endl;
        // }
    }

    // 调用GetFriendsList方法
    {
        friendservice::GetFriendsListRequest get_friends_request;
        friendservice::GetFriendsListResponse get_friends_response;
        get_friends_request.set_userid(25);

        MprpcController get_friends_controller;
        stub.GetFriendsList(&get_friends_controller, &get_friends_request, &get_friends_response, nullptr); 
        // 这个调用通过stub对象，实际上会调用MprpcChannel对象的CallMethod函数。

        if (get_friends_controller.Failed()) {
            std::cout << "GetFriendsList RPC failed: " << get_friends_controller.ErrorText() << std::endl;
        } else {
            std::cout << "GetFriendsList response: " << std::endl;
            for (int i = 0; i < get_friends_response.friends_size(); ++i) {
                const auto& friend_info = get_friends_response.friends(i);
                std::cout << "Friend ID: " << friend_info.id() << ", Name: " << friend_info.name() << ", State: " << friend_info.state() << std::endl;
            }
        }
    }
    

    return 0;
}
