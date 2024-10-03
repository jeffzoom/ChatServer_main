/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-23 09:40:07
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-10-01 16:32:14
 * @FilePath: /Linux_nc/ChatServer/ChatServer_main/mpRPC/example/callerchat/callofflinemessagemodel.cpp
 * @Descripttion: 
 */
#include <iostream>
#include "mprpcapplication.h"
// #include "mprpcchannel.h"
#include "offlinemessagemodel.pb.h"

int main(int argc, char** argv) {
    // 初始化RPC框架
    MprpcApplication::Init(argc, argv);

    // 创建OfflineMessageService_Stub
    offlinemsgservice::OfflineMessageService_Stub stub(new MprpcChannel());

    // 插入离线消息
    {
        offlinemsgservice::InsertRequest insertRequest;
        offlinemsgservice::InsertResponse insertResponse;
        insertRequest.set_userid(25);
        // insertRequest.set_message("Hello, this is a test message.");
        // const char *my_sql1 = "{"id":13,"msg":"hi","msgid":6,"name":"zhang san","time":"2024-07-18 23:07:53","toid":25}";
        const char *my_sql2 = "Hello, this is a test message.";
        insertRequest.set_message(my_sql2);

        MprpcController controller;
        stub.Insert(&controller, &insertRequest, &insertResponse, nullptr);
        if (controller.Failed()) {
            std::cout << "Insert failed: " << controller.ErrorText() << std::endl;
        } else {
            std::cout << "Insert response: " << insertResponse.message() << std::endl;
        }
    }

    // 查询离线消息
    {
        offlinemsgservice::QueryRequest queryRequest;
        offlinemsgservice::QueryResponse queryResponse;
        queryRequest.set_userid(211);

        MprpcController controller;
        stub.Query(&controller, &queryRequest, &queryResponse, nullptr);
        if (controller.Failed()) {
            std::cout << "Query failed: " << controller.ErrorText() << std::endl;
        } else {
            std::cout << "Query response: ";
            for (const auto& message : queryResponse.messages()) {
                std::cout << message << " ";
            }
            std::cout << std::endl;
        }
    }

    // 删除离线消息
    {
        // offlinemsgservice::RemoveRequest removeRequest;
        // offlinemsgservice::RemoveResponse removeResponse;
        // removeRequest.set_userid(25);

        // MprpcController controller;
        // stub.Remove(&controller, &removeRequest, &removeResponse, nullptr);
        // if (controller.Failed()) {
        //     std::cout << "Remove failed: " << controller.ErrorText() << std::endl;
        // } else {
        //     std::cout << "Remove response: " << removeResponse.message() << std::endl;
        // }
    }

    return 0;
}
