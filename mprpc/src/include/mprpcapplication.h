/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-07 16:25:01
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-21 22:09:32
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/include/mprpcapplication.h
 * @Descripttion: 
 */
#pragma once

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

// mprpc框架的基础类，负责框架的一些初始化操作
// 用单例来设计
class MprpcApplication {
public:
    static void Init(int argc, char **argv);
    static MprpcApplication& GetInstance();
    static MprpcConfig& GetConfig();

private:
    static MprpcConfig m_config;

    MprpcApplication() {}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};
