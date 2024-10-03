/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-07 20:38:35
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-21 22:48:46
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/mprpcapplication.cc
 * @Descripttion: 
 */
#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp() {
    std::cout<<"format: command -i <configfile>" << std::endl;
}

// 调用框架要首先显式的运行初始化函数
void MprpcApplication::Init(int argc, char **argv) {
    if (argc < 2) {
        ShowArgsHelp(); // 打印参数的帮助说明（配置文件）
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file; // 配置文件
    while((c = getopt(argc, argv, "i:")) != -1) {
        switch (c) {
        case 'i':
            config_file = optarg; // 配置文件的文件名test.conf
            break;
        case '?':
            // std::cout << "invalid args!" << std::endl; // 无效的args
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            // std::cout << "need <configfile>" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 开始加载配置文件了 rpcserver_ip=  rpcserver_port   zookeeper_ip=  zookepper_port=
    m_config.LoadConfigFile(config_file.c_str());

    // 测试代码
    // std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
    // std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
    // std::cout << "zookeeperip:" << m_config.Load("zookeeperip") << std::endl;
    // std::cout << "zookeeperport:" << m_config.Load("zookeeperport") << std::endl;
}

MprpcApplication& MprpcApplication::GetInstance() {
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig() {
    return m_config;
}
