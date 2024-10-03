/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-07 20:38:47
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-08-02 17:31:27
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/rpcprovider.cc
 * @Descripttion: 
 */
#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

// 借助RpcProvider类来发布一个本地服务，把本地服务发布成一个支持rpc调用的远程服务，
// 通过moduo和protubuf，只要200行代码  公司做项目，经常用到moduo和protubuf

/*
这样组织服务对象跟其方法的映射关系
service_name =>  service描述   
                        =》 service* 记录服务对象
                        method_name  =>  method方法对象
我们在网络通信的时候一定会选择一个数据协议，不可能没有数据协议，就通过字节流或者字符流其交互，没有办法交互
xml太老了  json存储键值对_文本存储   protobuf比json好用_二进制存储_protobuf存储数据不携带除数据之外的人和其他信息_效率更高_提供了rpc方法的描述
*/

// 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
// NotifyService是发布服务的站点   RpcProvider也可以叫RpcServer

// NotifyService功能：RpcProvider类发布本地服务，使其成为可通过 RPC 调用的远程服务。
// 步骤：
//     获取服务对象的描述信息。
//     获取服务名称service和方法数量。
//     遍历服务方法method，获取每个方法的描述信息，并将其记录在 service_info 的 m_methodMap 中。
//     将 service_info 存储在 m_serviceMap 中。
void RpcProvider::NotifyService(google::protobuf::Service *service) {
    ServiceInfo service_info;

    // 获取了服务对象的描述信息 （服务的描述ServiceDescriptor）
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    std::string service_name = pserviceDesc->name(); // 获取服务的名称
    int methodCnt = pserviceDesc->method_count(); // 获取服务对象service的方法的数量

    // std::cout << "service_name:" << service_name << std::endl; // 打印到控制台上的信息其实没有用处，而且打印多了也不好找
    LOG_INFO("service_name:%s", service_name.c_str()); // 打印服务的名字

    for (int i = 0; i < methodCnt; ++i) {
        // 获取了服务对象指定下标的服务方法的描述（抽象描述） UserService   Login 远端的RPC请求过来，要调用UserService服务对象的Login方法，method_name就会匹配，相当于就是switch case语句 （服务里方法的描述MethodDescriptor）
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc}); // 记录方法描述信息

        // std::cout << "method_name:" << method_name << std::endl;
        LOG_INFO("method_name:%s", method_name.c_str());
    }
    service_info.m_service = service; // 记录服务对象
    m_serviceMap.insert({service_name, service_info}); // 最终存储一下服务信息
}

// Run功能：启动 RPC 服务节点，提供远程网络调用服务。
// 步骤：
//     从配置文件读取 RPC 服务器的 IP 和端口。
//     创建 TcpServer 对象，绑定连接和消息回调函数。
//     设置 muduo 库的线程数量。
//     将注册的服务对象和方法名注册到 Zookeeper 上。
//     启动 TcpServer，开始网络服务。
// 启动rpc服务节点，开始提供rpc远程网络调用服务
// 把muduo库的启动都封装在Run方法中
void RpcProvider::Run() {
    // 读取配置文件rpcserver的信息
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip"); // 127.0.0.1   
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str()); // 8001
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    // 绑定连接回调和消息读写回调方法  用muduo的好处：分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1)); // OnConnection回调函数
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, 
            std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4); // 设置muduo库的线程数量

    // rpc服务的提供方，启动的时候把map里面注册的所有服务对象和服务方法名注册到zk上
    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // 默认session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    // 将服务注册到Zookeeper上
    ZkClient zkCli;
    zkCli.Start(); // 连接zkserver // 连接Zookeeper服务器
    // service_name为永久性节点    method_name为临时性节点
    for (auto &sp : m_serviceMap) {
        // /service_name   /UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_methodMap) {
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    // rpc服务端准备启动，打印信息
    std::cout << "RpcProvider start service at ip:" << ip << " port:" << port << std::endl;

    // 启动网络服务
    server.start();
    m_eventLoop.loop(); // 启动epoll_wait，以阻塞的方式等待远程链接 // 启动事件循环，有新连接，先运行OnConnection，然后运行OnMessage
    
}

// OnConnection和OnMessage不是自己调用，是moduo帮忙调用
// 新的socket连接回调 rpc连接属于短连接，和http一样
// OnConnection功能：处理新的连接。
// 步骤：
//     判断连接是否断开，如果断开则关闭连接。
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn) { // 右侧绿色 接收连接请求
    if (!conn->connected()) {
        // 和rpc client的连接断开了  在linux上一个socket就是一个文件描述符
        conn->shutdown(); // 关闭连接
    }
}

/*
在框架内部，RpcProvider(rpcServer)和RpcConsumer(rpcClient)协商好之间的通信用的protobuf数据类型
service_name methopd_name args   定义proto的message类型，进行数据头的序列化和反序列化
                                 head_str包含service_name, methopd_name, args_size
16UserServiceLoginzhang san123456   不要用加竖线|或别的方式，这种方式太垃圾了，上不了台面

header_size(4字节) + head_str + args_str
把service_name methopd_name这个消息头表示出来，剩下的就是参数args
来一长串字符串也不知道多长，我们要知道哪一截包含服务名和方法名

在tcp通信中还要考虑tcp粘包的问题，当前的请求和下一个连续的请求粘到一起了，
因此也要记录参数的长度

std::string   insert和copy方法 
*/
// 已建立连接用户的读写事件回调 如果远程有一个rpc服务的调用，那么OnMessage方法就会响应
// 发布rpc服务的节点从远端接收到一个rpc调用的请求
// OnMessage功能：处理接收到的 RPC 调用请求。
// 步骤：
//     解析接收到的字符流，获取消息头和参数。
//     根据消息头获取服务名称和方法名称。
//     查找服务对象和方法对象。
//     生成 RPC 方法调用的请求和响应参数。
//     调用服务对象的方法，并传递请求和响应参数。
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, 
                            muduo::net::Buffer *buffer, 
                            muduo::Timestamp)
{
    // 网络上接收的远程rpc调用请求的字符流    Login args
    std::string recv_buf = buffer->retrieveAllAsString(); // 右侧绿色 接收数据

    // 从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0); // 读取消息头大小

    // 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str)) { // ParseFromString数据头反序列化 // 反序列化消息头 // 右侧黄色 反序列化
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    } else {
        // 数据头反序列化失败
        std::cout << "rpc_header_str:" << rpc_header_str << " parse error!" << std::endl;
        return;
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, args_size); // 获取参数数据

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; // 重要 调用哪个服务对象
    std::cout << "method_name: " << method_name << std::endl; // 重要 调用哪个服务方法
    std::cout << "args_str: " << args_str << std::endl; // 重要 这个服务方法的参数
    std::cout << "============================================" << std::endl;

    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name); 
    if (it == m_serviceMap.end()) { // 该节点没有此服务
        std::cout << service_name << " is not exist!" << std::endl; 
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name); 
    if (mit == it->second.m_methodMap.end()) { // 服务对象的方法不存在
        std::cout << service_name << ":" << method_name << " is not exist!" << std::endl; 
        return;
    }

    google::protobuf::Service *service = it->second.m_service; // 获取service对象  new出来的UserService，provider.NotifyService(new UserService());      
    const google::protobuf::MethodDescriptor *method = mit->second; // 获取method对象，比如userservice.cc的Login

    // 生成rpc方法调用的请求request和响应response参数  从抽象层动态生成method请求和响应
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) { // 将参数args_str填到request请求里面，反序列化
        std::cout << "request parse error, content:" << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New(); // response是由业务来填的

    // 给下面的method方法的调用，绑定一个Closure的回调函数，用于序列化rpc的响应和网络发送，
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, 
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this, 
                                                                    &RpcProvider::SendRpcResponse, 
                                                                    conn, response);

    // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    // new UserService().Login(controller, request, response, done)
    // 不可能在框架上调用UserService对象的Login方法
    service->CallMethod(method, nullptr, request, response, done); // 在框架上调用业务的方法，然后业务中的方法，比如example中的userservice.cc中的void Login(::google::protobuf::RpcController* controller方法就会被调用
    // void Login(::google::protobuf::RpcController* controller方法在编译好后的"provider"可执行文件中运行
    // 运行Login得到的结果在通过 SendRpcResponse 发送给请求方

    // 发布者的 done->Run() 就是运行 SendRpcResponse
}

// Closure的回调操作，用于序列化rpc的响应和网络发送
// SendRpcResponse功能：序列化 RPC 响应并通过网络发送回调用方。发送RPC响应
// 步骤：
//     将响应序列化为字符串。
//     通过网络发送序列化后的响应。
//     模拟短连接服务，主动断开连接。
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message *response) {
    std::string response_str;
    if (response->SerializeToString(&response_str)) { // response进行序列化 右侧黄色 序列化
        // 序列化成功后，通过网络把rpc方法执行的结果发送会rpc的调用方 右侧绿色 发送数据
        conn->send(response_str);
    } else {
        std::cout << "serialize response_str error!" << std::endl; 
    }
    conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
}
