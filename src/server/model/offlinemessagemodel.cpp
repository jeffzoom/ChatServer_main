/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2023-10-12 15:56:37
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-18 23:33:51
 * @FilePath: /Linux_nc/ChatServer/myChatServer/src/server/model/offlinemessagemodel.cpp
 * @Descripttion: 
 */
#include "offlinemessagemodel.hpp"
// #include "db_chat.h"
#include "CommonConnectionPool.h"

// 存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg) {
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", userid, msg.c_str());

    // MySQL mysql;
    // if (mysql.connect()) {
    //     mysql.update(sql);
    // }
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    if (sp != nullptr) {
        sp->update(sql);
    }
}

// 删除用户的离线消息
void OfflineMsgModel::remove(int userid) {
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", userid);

    // MySQL mysql;
    // if (mysql.connect()) {
    //     mysql.update(sql);
    // }
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    if (sp != nullptr) {
        sp->update(sql);
    }
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid) {
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid);

    vector<string> vec;

    // MySQL mysql;
    // if (mysql.connect()) {
    //     MYSQL_RES *res = mysql.query(sql);

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    if (sp != nullptr) {
        MYSQL_RES* res = sp->query(sql);
        
        if (res != nullptr) {
            // 把userid用户的所有离线消息放入vec中返回
            MYSQL_ROW row;
            // 循环，因为mysql里可能存在多条数据
            while((row = mysql_fetch_row(res)) != nullptr) {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}
