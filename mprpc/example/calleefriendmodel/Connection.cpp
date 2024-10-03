/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-17 18:59:18
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-18 18:57:14
 * @FilePath: /Linux_nc/CommonConnectionPool/shilei/Connection.cpp
 * @Descripttion: 
 */

// #include "pch.h"
#include "public.h"
#include "Connection.h"
#include <iostream>
using namespace std;

Connection::Connection() {
	_conn = mysql_init(nullptr); // 初始化数据库连接
}

Connection::~Connection() {
	if (_conn != nullptr) {
		mysql_close(_conn); // 释放数据库连接资源
	}
}

bool Connection::connect(string ip, unsigned short port, 
	string username, string password, string dbname) 
{
	// 连接数据库
	MYSQL *p = mysql_real_connect(_conn, ip.c_str(), username.c_str(),
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	if (p == nullptr) LOG("连接失败: " + string(mysql_error(_conn))); // 连接失败
	return p != nullptr; // p不为空返回true，p为空返回false
}

// 增，删，改 // 更新操作 insert、delete、update
bool Connection::update(string sql) {
	if (mysql_query(_conn, sql.c_str())) {
		LOG("更新失败:" + sql);
		cout << mysql_error(_conn) << endl;
		return false;
	}
	return true;
}

// 查 // 查询操作 select
MYSQL_RES* Connection::query(string sql) {
	if (mysql_query(_conn, sql.c_str())) {
		// LOG("查询失败:" + sql);
		LOG("查询失败: " + string(mysql_error(_conn)) + " : " + sql);
		cout << mysql_error(_conn) << endl;
		return nullptr;
	}
	// cout << "query ok" << endl;
	return mysql_use_result(_conn);
}

// 打印查询结果
void Connection::printResult(MYSQL_RES* res) {
	if (res == nullptr) return;
	int num_fields = mysql_num_fields(res);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		for (int i = 0; i < num_fields; i++) {cout << (row[i] ? row[i] : "NULL") << "\t";}
		cout << endl;
	}
	mysql_free_result(res); // 释放一个结果集合使用的内存
}
