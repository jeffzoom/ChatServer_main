/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-17 18:59:18
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-18 18:53:02
 * @FilePath: /Linux_nc/CommonConnectionPool/shilei/public.h
 * @Descripttion: 
 */
#ifndef _PUBLIC_H_
#define _PUBLIC_H_

// 打印示例：Connection.cpp:51 Wed Jul 17 21:34:17 2024 : 查询失败:SELECT * FROM user
#define LOG(str) \
	cout << __FILE__ << ":" << __LINE__ << " " << \
	__TIMESTAMP__ << " : " << str << endl;

#endif