# 定义了一个SRC_LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis REDIS_LIST)
aux_source_directory(./rpc_proto_cc RPC_PROTO_CC_LIST)

# 指定生成可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST} ${RPC_PROTO_CC_LIST})

# 添加libmprpc.a库文件的绝对路径
set(MPRPC_LIB /home/nowcoder/Linux_nc/ChatServer/myChatServer/src/server/libmprpc.a)

# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread ${MPRPC_LIB} zookeeper_mt protobuf)

# 在target_link_libraries中没有添加hiredis，然后就会报错：
# CMakeFiles/ChatServer.dir/redis/redis.cpp.o：在函数‘Redis::~Redis()’中：
# /home/nowcoder/Linux_nc/ChatServer/myChatServer/src/server/redis/redis.cpp:21：对‘redisFree’未定义的引用
# /home/nowcoder/Linux_nc/ChatServer/myChatServer/src/server/redis/redis.cpp:25：对‘redisFree’未定义的引用