/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-22 23:16:57
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-23 19:30:38
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/calleeusermodel/testusermodel.cpp
 * @Descripttion: 
 */
#include <iostream>
#include <string>
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "usermodel.hpp"
#include "usermodel.pb.h"  // 包含由Protobuf生成的头文件

class UserServiceImpl : public userservice::UserService {
public:
    // 实现 Insert 方法
    void Insert(::google::protobuf::RpcController* controller,
                const ::userservice::InsertRequest* request,
                ::userservice::InsertResponse* response,
                ::google::protobuf::Closure* done) override {
        UserModel model;
        User user;
        user.setId(request->user().id());
        user.setName(request->user().name());
        user.setPwd(request->user().password());
        user.setState(request->user().state());
        
        bool success = model.insert(user);
        response->set_success(success);
        response->set_message(success ? "User inserted successfully." : "Failed to insert user.");
        done->Run();
    }

    // 实现 Query 方法
    void Query(::google::protobuf::RpcController* controller,
               const ::userservice::QueryRequest* request,
               ::userservice::QueryResponse* response,
               ::google::protobuf::Closure* done) override {
        UserModel model;
        User user = model.query(request->id());
        if (user.getId() != -1) {
            auto* user_info = response->mutable_user();
            user_info->set_id(user.getId());
            user_info->set_name(user.getName());
            user_info->set_password(user.getPwd());
            user_info->set_state(user.getState());
            response->set_found(true);
        } else {
            response->set_found(false);
        }
        done->Run();
    }

    // 实现 UpdateState 方法
    void UpdateState(::google::protobuf::RpcController* controller,
                     const ::userservice::UpdateStateRequest* request,
                     ::userservice::UpdateStateResponse* response,
                     ::google::protobuf::Closure* done) override {
        UserModel model;
        User user;
        user.setId(request->user().id());
        user.setState(request->user().state());
        
        bool success = model.updateState(user);
        response->set_success(success);
        done->Run();
    }

    // 实现 ResetState 方法
    void ResetState(::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Empty* request,
                    ::userservice::ResetStateResponse* response,
                    ::google::protobuf::Closure* done) override {
        UserModel model;
        model.resetState();
        response->set_success(true);
        done->Run();
    }
};

int main(int argc, char** argv) {
    // 初始化RPC框架
    MprpcApplication::Init(argc, argv);

    // 创建并注册UserService服务
    RpcProvider provider;
    provider.NotifyService(new UserServiceImpl());

    // 启动RPC服务发布节点
    provider.Run();

    return 0;
}
