/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-17 11:07:58
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 13:31:08
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/include/mprpccontroller.h
 * @Descripttion: 
 */
#pragma once
#include <google/protobuf/service.h>
#include <string>

class MprpcController : public google::protobuf::RpcController {
public:
    MprpcController();
    void Reset();                   // 重置
    bool Failed() const;            // 检查是否失败，返回true就是发生问题了
    std::string ErrorText() const;  // 获取错误文本
    void SetFailed(const std::string& reason);  // 设置失败信息

    // 目前未实现具体的功能
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);

private:
    bool m_failed; // RPC方法执行过程中的状态，rpc调用过程成功与否
    std::string m_errText; // RPC方法执行过程中的错误信息，调用失败的错误信息
};
