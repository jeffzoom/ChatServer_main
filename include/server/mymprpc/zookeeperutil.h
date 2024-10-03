/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-18 15:18:59
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-02-18 19:12:07
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/include/zookeeperutil.h
 * @Descripttion: 
 */
#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

// 封装的zk客户端类
class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    // zkclient启动连接zkserver
    void Start();
    // 在zkserver上根据指定的path创建znode节点  znode节点的路径 节点的数据 数据的长度 0永久性节点(默认)
    void Create(const char *path, const char *data, int datalen, int state=0);
    // 根据参数指定的znode节点路径，或者znode节点的值
    std::string GetData(const char *path);
private:
    // zk的客户端句柄  通过这个句柄可以操作zkserver
    zhandle_t *m_zhandle; 
};