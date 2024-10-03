/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-10 18:42:48
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-21 22:18:17
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/include/mprpcconfig.h
 * @Descripttion: 
 */
#pragma once

#include <unordered_map>
#include <string>

// rpcserverip   rpcserverport    zookeeperip   zookeeperport
// 框架读取配置文件类
class MprpcConfig {
public:
    // 负责解析加载配置文件
    void LoadConfigFile(const char *config_file);
    // 查询配置项信息
    std::string Load(const std::string &key);
    
private:
    std::unordered_map<std::string, std::string> m_configMap;
    // 去掉字符串前后的空格
    void Trim(std::string &src_buf);
};