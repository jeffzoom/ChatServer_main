aux_source_directory(. SRC_LIST) # 当前目录的所有源文件名字放入SRC_LIST
# cmake改动了，才会生成新的makefile，因此上面这样写的话 cmake.. make编译不会出错
# 不过在用vscode一键编译的时候，因为cmake没有改动，因为不会生成新的makefile
# 因此可能会出现在连接link时出现某些函数未定义的错误
# 如果不用cmake.. make，非得用vscode一键编译的话，就去？build目录下面，把所有的文件删掉，重新生成

# set(SRC_LIST 
#     mprpcapplication.cc 
#     mprpcconfig.cc 
#     rpcheader.pb.cc 
#     rpcprovider.cc 
#     mprpcchannel.cc
#     mprpccontroller.cc
#     logger.cc
#     zookeeperutil.cc)

# add_library(mprpc SHARED ${SRC_LIST}) # 生成动态库
add_library(mprpc ${SRC_LIST}) # 生成静态库
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt) 
# zookeeper_mt是多线程版本  zookeeper_st是单线程版本 一般都用多线程版本
# target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)
# sudo find /usr -name "libmuduo*"
# muduo生成的是.a 是静态库
# 在Windows上，静态库的文件扩展名通常是.lib。动态库的文件扩展名通常是.dll（Dynamic Link Library）。
# 在Linux/Unix上，静态库的文件扩展名通常是.a。动态库的文件扩展名通常是.so（Shared Object）。