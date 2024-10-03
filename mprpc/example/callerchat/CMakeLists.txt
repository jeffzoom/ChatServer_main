# 也可以建四个文件夹，这样都能编译

set(SRC_LIST calltestfriendmodel.cpp ../friendmodel.pb.cc)
add_executable(calltestfriendmodel ${SRC_LIST})
target_link_libraries(calltestfriendmodel mprpc protobuf) # 链接mprpc动态库 链接框架的.so库

# set(SRC_LIST callusermodel.cpp ../usermodel.pb.cc)
# add_executable(callusermodel ${SRC_LIST})
# target_link_libraries(callusermodel mprpc protobuf) # 链接mprpc动态库 链接框架的.so库

# set(SRC_LIST callofflinemessagemodel.cpp ../offlinemessagemodel.pb.cc)
# add_executable(callofflinemessagemodel ${SRC_LIST})
# target_link_libraries(callofflinemessagemodel mprpc protobuf) # 链接mprpc动态库 链接框架的.so库

# set(SRC_LIST callgroupmodel.cpp ../groupmodel.pb.cc)
# add_executable(callgroupmodel ${SRC_LIST})
# target_link_libraries(callgroupmodel mprpc protobuf) # 链接mprpc动态库 链接框架的.so库
