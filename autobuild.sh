
###
 # @version: 
 # @Author: zsq 1363759476@qq.com
 # @Date: 2024-02-25 20:33:51
 # @LastEditors: zsq 1363759476@qq.com
 # @LastEditTime: 2024-02-25 20:35:19
 # @FilePath: /Linux_nc/ChatServer/myChatServer/autobuild1.sh
 # @Descripttion: 
### 
#!/bin/bash

set -x

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make
