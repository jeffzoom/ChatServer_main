/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2023-10-12 21:26:27
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2023-10-13 10:39:13
 * @FilePath: /Linux_nc/ChatServer/myChatServer/include/server/friendmodel.hpp
 * @Descripttion: 
 */
#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include <vector>
using namespace std;

// 维护好友信息的操作接口方法
class FriendModel
{
public:
    // 添加好友关系
    void insert(int userid, int friendid);

    // 返回用户好友列表
    vector<User> query(int userid);
};

#endif