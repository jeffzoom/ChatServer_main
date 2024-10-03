# set(SRC_LIST userservice.cc ../user.pb.cc)
# set(SRC_LIST friendservice.cc ../friend.pb.cc)
# set(SRC_LIST testfriend.cpp friendmodel.cpp CommonConnectionPool.cpp Connection.cpp)
set(SRC_LIST testofflinemessagemodel.cpp offlinemessagemodel.cpp ../calleefriendmodel/CommonConnectionPool.cpp ../calleefriendmodel/Connection.cpp ../offlinemessagemodel.pb.cc)

add_executable(testofflinemessagemodel ${SRC_LIST})
# target_link_libraries(provider mprpc protobuf) # 链接mprpc动态库 链接框架的.so库 user.pb.cc或friend.pb.cc
target_link_libraries(testofflinemessagemodel mprpc protobuf mysqlclient pthread) # 链接mprpc动态库 链接框架的.so库 friendmodel测试
