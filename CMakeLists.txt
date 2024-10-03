# 必须在build下运行此CMakeLists.txt，在build下生成Makefile，make运行Makefile
cmake_minimum_required (VERSION 3.0) #要求cmake最低的版本号
project (chat) # 定义当前工程名字

# 配置编译选项，将调试信息生成标志 -g 添加到 C++ 编译器的标志中
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/include/server/mymprpc)
include_directories(${PROJECT_SOURCE_DIR}/include/server/rpc_proto_h)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
# link_directories(/usr/lib64/mysql)

# 加载子目录src
add_subdirectory(src)
