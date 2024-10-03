/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-18 17:41:56
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-08-02 15:51:27
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/zookeeperutil.cc
 * @Descripttion: 
 */
#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>

// zhandle_t *zh当前客户端的句柄
// 全局的watcher观察器   zkserver给zkclient的通知，zkclient之前发送过消息给zkserver，
// zkserver收到并处理之后，zkserver通过zkclient事先注册的回调来给zkclient发通知
// global_watcher功能：全局观察器，处理 Zookeeper 服务器的通知。
// 步骤：
//     判断消息类型是否与会话相关。
//     如果是连接成功，则增加信号量资源。
void global_watcher(zhandle_t *zh, int type,
                	int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT) { // 回调的消息类型是和会话相关的消息类型  连接，断开连接，会话超时
		if (state == ZOO_CONNECTED_STATE) { 			// 若zkclient和zkserver连接成功，通过信号量+1的方式通知
			sem_t *sem = (sem_t*)zoo_get_context(zh); 	// 从指定的句柄上获取信号量
            sem_post(sem); 								// 然后给这个信号量资源+1 // 增加信号量
		}
	}
}

ZkClient::ZkClient() : m_zhandle(nullptr) {}

ZkClient::~ZkClient() {
    if (m_zhandle != nullptr) { // 如果指针不为空，那就说明已经和zkserver连接，之后要关闭掉
        zookeeper_close(m_zhandle); // 关闭句柄，释放资源  MySQL_Conn // 关闭Zookeeper句柄
    }
}

// 连接zkserver
// Start功能：连接 Zookeeper 服务器。被rpcprovider.cc和mprpcchannel.cc调用
// 步骤：
//     从配置文件读取 Zookeeper 服务器的 IP 和端口。
//     初始化 Zookeeper 客户端句柄并设置全局观察器。
//     使用信号量等待连接成功。
void ZkClient::Start() {
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip"); // 127.0.0.1
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport"); // 2181
    std::string connstr = host + ":" + port; // 这个格式在zookeeper_init中指定了
    
	/*
	用zookeeper_mt多线程版本的原因
	zookeeper的API客户端程序提供了三个线程
	1.API调用线程，也就是当前线程
	2.网络I/O线程，启了一个pthread_create，专门在一个线程做网络io操作，底层用的poll，毕竟zookeeper_init是个客户端程序，不用高并发
	3.watcher回调线程，pthread_create
	*/
	// global_watcher 是回调函数  30000是会话的超时时间  nullptr没有用
	// 这个会话的创建时异步的， zookeeper_init 的注释都有写，Session establishment is asynchronous
	m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (nullptr == m_zhandle) {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);
    }
	// 创建句柄资源成功，并不是连接zkserver成功

    sem_t sem; // 信号量
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem); // 将用户特定的上下文数据与ZooKeeper客户端句柄关联起来，给指定的句柄添加一些额外的信息

    sem_wait(&sem); // 等待连接成功 // 主线程，阻塞，因为初始的信号量是0，我当然要阻塞在这里，因为我需要知道连接zkserver是否成功
	// 等sem_post函数+1，看到信号量有资源，就执行下一行
    std::cout << "zookeeper_init success!" << std::endl;
}

// path每个节点都有一个path，data是节点存储的数据，datalen是数据的长度，state是临时性节点还是持久性节点
// Create功能：创建 Zookeeper 节点。被rpcprovider.cc调用
// 步骤：
//     判断节点是否存在，如果不存在则创建节点。
//     根据状态参数决定创建临时节点或持久节点。
void ZkClient::Create(const char *path, const char *data, int datalen, int state) {
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag;
	// 先判断path表示的znode节点是否存在，如果存在，就不再重复创建了
	flag = zoo_exists(m_zhandle, path, 0, nullptr);
	if (ZNONODE == flag) { // 表示path的znode节点不存在
		// 创建指定path的znode节点了，根据state判断是创建临时性节点还是永久性节点
		flag = zoo_create(m_zhandle, path, data, datalen,
			&ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen); // m_zhandle是句柄
		if (flag == ZOK) { // 异常判断
			std::cout << "znode create success... path:" << path << std::endl; // 要放到日志
		} else { // 异常判断
			std::cout << "flag:" << flag << std::endl; // 要放到日志
			std::cout << "znode create error... path:" << path << std::endl; // 要放到日志
			exit(EXIT_FAILURE);
		}
	}
}

// 想访问一个服务，那我需要先得到这个服务所在的ip地址和端口号
// 根据指定的path，获取znode节点的值
// GetData功能：获取指定路径节点的数据。被mprpcchannel.cc调用
// 步骤：
//     获取节点数据，并返回数据字符串。
std::string ZkClient::GetData(const char *path) {
    char buffer[64];
	int bufferlen = sizeof(buffer);
	int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
	if (flag != ZOK) { // ZOK operation completed successfully 不等于ZOK表示操作失败
		std::cout << "get znode error... path:" << path << std::endl; // 要放到日志
		return "";
	} else {
		return buffer;
	}
}
