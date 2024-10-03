#!/bin/bash
###
 # @version: 
 # @Author: zsq 1363759476@qq.com
 # @Date: 2024-02-06 13:20:53
 # @LastEditors: zsq 1363759476@qq.com
 # @LastEditTime: 2024-02-19 12:12:28
 # @FilePath: /Linux_nc/mpRPC/mympRPC/autobuild.sh
 # @Descripttion: 
### 

# 在mymprpc目录下运行shell

set -e

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make
cd ..
cp -r `pwd`/src/include `pwd`/lib  # 要拷贝到lib目录下，要给用户提供头文件
# 这个框架使用的时候需要给人家提供头文件，没有头文件人家怎么知道我们这个框架提供什么样的API呢？对不对啊？
# 其次就是要把这个静态库libmprpc.a给到人家
#
# libmprpc可以放到user/lib/或者user/local/lib/，
# include头文件可以放到user/include/或者user/local/include/下，
# 这样别人用的话就可以默认查找到，否则要链接这个库，找这个头文件，还得去指定库的路径和头文件的路径
#
# 可以用lib下的静态库libmprpc.a和头文件include/重新写一个示例
