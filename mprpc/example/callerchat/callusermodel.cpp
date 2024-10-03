/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-22 23:25:05
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-23 19:34:24
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/callerchat/callusermodel.cpp
 * @Descripttion: 
 */
#include <iostream>
#include "mprpcapplication.h"
#include "usermodel.pb.h"
#include "mprpcchannel.h"
#include "user.hpp"

int main(int argc, char** argv) {
    // 初始化RPC框架
    MprpcApplication::Init(argc, argv);

    // 创建UserServiceRpc_Stub
    userservice::UserService_Stub stub(new MprpcChannel());

    // 插入用户 没问题
    {
        // userservice::InsertRequest insertRequest;
        // userservice::InsertResponse insertResponse;

        // // 手动转换 User 对象到 userservice::User 对象
        // userservice::User* user_info = insertRequest.mutable_user();
        // user_info->set_name("JohnDoe2");
        // user_info->set_password("123456");
        // user_info->set_state("offline");

        // MprpcController controller;
        // stub.Insert(&controller, &insertRequest, &insertResponse, nullptr);
        // if (controller.Failed()) {
        //     std::cout << "Insert failed: " << controller.ErrorText() << std::endl;
        // } else {
        //     std::cout << "Insert response: " << insertResponse.message() << std::endl;
        // }
    }

    // 查询用户
    {
        userservice::QueryRequest queryRequest;
        userservice::QueryResponse queryResponse;
        queryRequest.set_id(27);

        MprpcController controller;
        stub.Query(&controller, &queryRequest, &queryResponse, nullptr);
        if (controller.Failed()) {
            std::cout << "Query failed: " << controller.ErrorText() << std::endl;
        } else {
            if (queryResponse.found()) {
                std::cout << "User found: " << queryResponse.user().name() << std::endl; // User found: JohnDoe
                std::cout << std::endl;
                std::cout << std::endl;
                std::cout << "hhhhhhhhhhhhhh" << std::endl;
                std::cout << std::endl;
                std::cout << std::endl;
            } else {
                std::cout << "User not found." << std::endl;
            }
        }
    }

    // 更新单一用户状态
    {
        userservice::UpdateStateRequest updateRequest;
        userservice::UpdateStateResponse updateResponse;

        userservice::User* user_info = updateRequest.mutable_user();
        user_info->set_id(27);
        user_info->set_state("offline");

        MprpcController controller;
        stub.UpdateState(&controller, &updateRequest, &updateResponse, nullptr);
        if (controller.Failed()) {
            std::cout << "UpdateState failed: " << controller.ErrorText() << std::endl;
        } else {
            std::cout << "UpdateState response: " << (updateResponse.success() ? "Success" : "Failure") << std::endl;
        }
    }

    // 重置所有用户状态
    {
        // 运行testusermodel.cpp中的ResetState中的model.resetState()
        // model.resetState()中的sql命令是update user set state = 'offline' where state = 'online'
        google::protobuf::Empty resetRequest;
        userservice::ResetStateResponse resetResponse;

        MprpcController controller;
        stub.ResetState(&controller, &resetRequest, &resetResponse, nullptr);
        if (controller.Failed()) {
            std::cout << "ResetState failed: " << controller.ErrorText() << std::endl;
        } else {
            std::cout << "ResetState response: " << (resetResponse.success() ? "Success" : "Failure") << std::endl;
        }
    }

    return 0;
}
