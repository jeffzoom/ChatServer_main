/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2023-10-13 10:22:47
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-18 23:38:24
 * @FilePath: /Linux_nc/ChatServer/myChatServer/src/server/model/groupmodel.cpp
 * @Descripttion: 
 */
#include "groupmodel_z.hpp"
// #include "db_chat.h"
#include "CommonConnectionPool.h"

// 创建群组
bool GroupModel::createGroup(Group &group) {
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')",
            group.getName().c_str(), group.getDesc().c_str());

    // MySQL mysql;
    // if (mysql.connect()) {
    //     if (mysql.update(sql)) {
    //         group.setId(mysql_insert_id(mysql.getConnection())); // 得到自动增长的mysql给分配的群组id
    //         // mysql_insert_id(mysql.getConnection())把主键id拿出来，set到group里面，把实参group的id修改正确
    //         return true;
    //     }
    // }

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    if (sp != nullptr) {
        if (sp->update(sql)) {
            group.setId(mysql_insert_id(sp->getConnection())); // 得到自动增长的mysql给分配的群组id
            // mysql_insert_id(sp->getConnection())把主键id拿出来，set到group里面，把实参group的id修改正确
            return true;
        }
    }

    return false;
}

// 加入群组
void GroupModel::addGroup(int userid, int groupid, string role) {
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')",
            groupid, userid, role.c_str());

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

// 查询用户所在群组信息 给某一个成功登录的用户查看当前用户号，昵称，在线状态，群管理还是群员
vector<Group> GroupModel::queryGroups(int userid) {
    /*
    1. 先根据userid在groupuser表中查询出该用户所属的群组信息
    2. 在根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查出用户的详细信息
    */
    // 把指定的用户userid所在群组的详细信息全部查出来，不要多次查询，数据库的压力可是很大的
    // sql语句可以从十几秒优化到毫秒级，sql不能随便写，不是说能在数据库捞到数据就行，sql很讲究
    // 在服务器应用代码上改代码来提高服务器的效率很难，但是把复杂的sql语句写好提高整体服务器效率很容易
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join \
            groupuser b on a.id = b.groupid where b.userid=%d",
            userid);

    vector<Group> groupVec;

    // MySQL mysql;
    // if (mysql.connect()) {
    //     MYSQL_RES *res = mysql.query(sql);
    
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    if (sp != nullptr) {
        MYSQL_RES* res = sp->query(sql);

        if (res != nullptr) {
            MYSQL_ROW row;

            // 查出userid所有的群组信息
            while ((row = mysql_fetch_row(res)) != nullptr) {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    // 查询群组的用户信息
    for (Group &group : groupVec) {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from user a \
                inner join groupuser b on b.userid = a.id where b.groupid=%d",
                group.getId());

        // MYSQL_RES *res = mysql.query(sql);
        MYSQL_RES* res = sp->query(sql);
        if (res != nullptr) {

            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

// 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid) {
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);

    vector<int> idVec;

    // MySQL mysql;
    // if (mysql.connect()) {
    //     MYSQL_RES *res = mysql.query(sql);

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    shared_ptr<Connection> sp = cp->getConnection();
    if (sp != nullptr) {
        MYSQL_RES* res = sp->query(sql);

        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}