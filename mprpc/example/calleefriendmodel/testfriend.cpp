/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-22 21:41:34
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 21:49:58
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/callee/testfriend.cpp
 * @Descripttion: 
 */
#include "friendmodel.hpp"
#include <iostream>

int main() {
    FriendModel testaddfriend;
    vector<User> all = testaddfriend.query(13);
    for (int i = 0; i < all.size(); i++) {
        cout << all[i].getId() << endl;
    }
    return 0;
}
