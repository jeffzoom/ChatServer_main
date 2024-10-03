/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-15 17:27:04
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 14:00:24
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/mprpcchannel.cc
 * @Descripttion: 
 */
#include "mprpcchannel.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "mprpccontroller.h"
#include "zookeeperutil.h"

/*
header_size + service_name method_name args_size + args
*/
// 所有通过stub代理对象调用的rpc方法，都走到这里了，统一做rpc方法调用的数据数据序列化和网络发送 
// 这是一个负责处理 RPC 请求的通道类。
// 该类主要用于序列化请求数据、发送请求、接收响应并反序列化响应数据
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                              google::protobuf::RpcController* controller, 
                              const google::protobuf::Message* request,
                              google::protobuf::Message* response,
                              google::protobuf:: Closure* done)
{
    // 获取服务名称和方法名称
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name(); // service_name
    std::string method_name = method->name(); // method_name

    // 获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        // std::cout << "serialize request error!" << std::endl; // 后面需要把错误提示加到日志上
        controller->SetFailed("serialize request error!");
        return;
    }

    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    // 把RpcHeader填好之后进行序列化成字符串
    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str)) { // 序列化 左侧黄色
        header_size = rpc_header_str.size();
    } else {
        // std::cout << "serialize rpc header error!" << std::endl;
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    // 组织待发送的rpc请求的字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4)); // header_size 前四个字节
    send_rpc_str += rpc_header_str; // rpcheader
    send_rpc_str += args_str; // args

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "============================================" << std::endl;

    // 左侧绿色
    // 使用tcp编程，完成rpc方法的远程调用
    // 创建socket套接字并连接到 RPC 服务节点
    int clientfd = socket(AF_INET, SOCK_STREAM, 0); // 如果close(clientfd)麻烦，可以创建智能指针，动态维护一个clientfd，然后自定义删除器
    if (-1 == clientfd) {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
        // std::cout << "create socket error! errno:" << errno << std::endl;
        // exit(EXIT_FAILURE);
    }

    // 读取配置文件rpcserver的信息
    // std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    //
    // 从zk上查想要的服务所在的ip地址和端口号
    // rpc调用方想调用service_name的method_name服务，需要查询zk上该服务所在的host信息
    ZkClient zkCli;
    zkCli.Start();
    //  /UserServiceRpc/Login
    std::string method_path = "/" + service_name + "/" + method_name;
    // 127.0.0.1:8001
    std::string host_data = zkCli.GetData(method_path.c_str());
    if (host_data == "") {
        controller->SetFailed(method_path + " is not exist!"); // 出错信息
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1) {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str()); 
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) { // 阻塞，直到连接成功
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
        // std::cout << "connect error! errno:" << errno << std::endl;
        // close(clientfd);
        // exit(EXIT_FAILURE);
    }

    // 发送rpc请求
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0)) { // 阻塞，直到发送完毕
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
        // std::cout << "send error! errno:" << errno << std::endl;
        // close(clientfd);
        // return;
    }

    // 接收rpc请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))) { // 阻塞，直到接收到响应数据
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
        // std::cout << "recv error! errno:" << errno << std::endl;
        // close(clientfd);
        // return;
    }

    // 左侧黄色 反序列化
    // 反序列化响应值，反序列化rpc调用的响应数据
    // std::string response_str(recv_buf, 0, recv_size); // bug出现问题，recv_buf中遇到\0后面的数据就存不下来了，导致反序列化失败
    // if (!response->ParseFromString(response_str)) // 把字符串中的数据填到response里面
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        // std::cout << "parse error! response_str:" << recv_buf << std::endl;
        // close(clientfd);
        // return;
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse error! response_str:%s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    close(clientfd);

}
