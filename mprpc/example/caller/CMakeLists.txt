# set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)

# set(SRC_LIST calltestfriendmodel2.cpp ../friendmodel.pb.cc)

add_executable(consumer ${SRC_LIST})
target_link_libraries(consumer mprpc protobuf) # 链接mprpc动态库 链接框架的.so库
