/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2023-10-12 21:26:47
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-22 21:45:38
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/callee/friendmodel.cpp
 * @Descripttion: 
 */

#include "friendmodel.hpp"
// #include "db_chat.h"
#include "CommonConnectionPool.h"

 // 添加好友关系
void FriendModel::insert(int userid, int friendid) {
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    if (sp != nullptr) {
        sp->update(sql);
    }
}

/*
    mysql> select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=13;
    +----+----------+---------+
    | id | name     | state   |
    +----+----------+---------+
    | 15 | li si    | offline |
    | 16 | liu shuo | offline |
    | 21 | gao yang | offline |
    +----+----------+---------+
*/
// 返回用户好友列表
vector<User> FriendModel::query(int userid) {
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d", userid);
    
    vector<User> vec;

    // MySQL mysql;
    // if (mysql.connect()) {
    //     MYSQL_RES *res = mysql.query(sql); // 数据库查询操作

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    if (sp != nullptr) {
        MYSQL_RES* res = sp->query(sql);

        if (res != nullptr)  {
            // 把userid用户的所有离线消息放入vec中返回
            MYSQL_ROW row;
            // mysql_fetch_row()函数从指定的结果集中获取一行数据返回给row，是数组的形式，即row内部是字符串数组指针（二级指针）
            while((row = mysql_fetch_row(res)) != nullptr) { // 找到之后
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res); // 释放资源
            return vec;
        }
    }
    return vec;
}
