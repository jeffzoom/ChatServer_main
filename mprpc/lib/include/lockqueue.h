/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-02-17 16:51:27
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-02-17 23:19:32
 * @FilePath: /Linux_nc/mpRPC/mympRPC/src/include/lockqueue.h
 * @Descripttion: 
 */
#pragma once
#include <queue>
#include <thread>
#include <mutex> // pthread_mutex_t 互斥锁
#include <condition_variable> // pthread_condition_t 条件变量
// 线程间的同步通信有三种：互斥锁，二元信号量。线程间通信：条件变量，多元信号量

// 异步写日志的日志队列
template<typename T> // 模板的话只能在头文件定义，不能在源文件定义
class LockQueue
{
public:
    // 多个worker线程都会写日志queue 
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(m_mutex); // lock_guard可以自动的加锁释放锁，类似智能指针
        m_queue.push(data);
        m_condvariable.notify_one();
    }
    // 出中括号，锁就自动释放了

    // 一个线程读日志queue，写日志文件
    // T& Pop() // 不可以返回data的引用，data是函数内部的局部变量，返回引用之后data就销毁了，返回的引用就是一个野引用了
    T Pop() 
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty())
        {
            // 日志队列为空，线程进入wait状态
            m_condvariable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};