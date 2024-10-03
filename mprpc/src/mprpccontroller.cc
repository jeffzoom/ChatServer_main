/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-17 11:08:04
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 13:26:14
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/mprpccontroller.cc
 * @Descripttion: 
 */
#include "mprpccontroller.h"

MprpcController::MprpcController() {
    m_failed = false;
    m_errText = "";
}

// 重置
void MprpcController::Reset() {
    m_failed = false;
    m_errText = "";
}

// 检查是否失败，返回true就是发生问题了
bool MprpcController::Failed() const {
    return m_failed;
}

// 获取错误文本
std::string MprpcController::ErrorText() const {
    return m_errText;
}

// 设置失败信息
void MprpcController::SetFailed(const std::string& reason) {
    m_failed = true;
    m_errText = reason; // 记录发生错误的具体信息
}

// 目前未实现具体的功能
void MprpcController::StartCancel(){}
bool MprpcController::IsCanceled() const {return false;}
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}
