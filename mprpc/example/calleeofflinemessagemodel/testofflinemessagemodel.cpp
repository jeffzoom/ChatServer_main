/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-23 09:38:13
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-23 09:38:29
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/calleeofflinemessagemodel/testofflinemessagemodel.cpp
 * @Descripttion: 
 */
#include "offlinemessagemodel.pb.h"
#include "offlinemessagemodel.hpp"
#include "mprpcapplication.h"
#include "rpcprovider.h"

class OfflineMessageServiceImpl : public offlinemsgservice::OfflineMessageService {
public:
    void Insert(::google::protobuf::RpcController* controller,
                const ::offlinemsgservice::InsertRequest* request,
                ::offlinemsgservice::InsertResponse* response,
                ::google::protobuf::Closure* done) override {
        OfflineMsgModel model;
        model.insert(request->userid(), request->message());
        response->set_success(true);
        response->set_message("Insert successful");
        done->Run();
    }

    void Remove(::google::protobuf::RpcController* controller,
                const ::offlinemsgservice::RemoveRequest* request,
                ::offlinemsgservice::RemoveResponse* response,
                ::google::protobuf::Closure* done) override {
        OfflineMsgModel model;
        model.remove(request->userid());
        response->set_success(true);
        response->set_message("Remove successful");
        done->Run();
    }

    void Query(::google::protobuf::RpcController* controller,
               const ::offlinemsgservice::QueryRequest* request,
               ::offlinemsgservice::QueryResponse* response,
               ::google::protobuf::Closure* done) override {
        OfflineMsgModel model;
        std::vector<std::string> messages = model.query(request->userid());
        for (const std::string& msg : messages) {
            response->add_messages(msg);
        }
        done->Run();
    }
};

int main(int argc, char** argv) {
    // 初始化RPC框架
    MprpcApplication::Init(argc, argv);

    // 创建并注册OfflineMessageService服务
    RpcProvider provider;
    provider.NotifyService(new OfflineMessageServiceImpl());

    // 启动RPC服务发布节点
    provider.Run();

    return 0;
}
