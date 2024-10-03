/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-23 10:24:42
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-10-01 16:32:03
 * @FilePath: /Linux_nc/ChatServer/ChatServer_main/mpRPC/example/callerchat/callgroupmodel.cpp
 * @Descripttion: 
 */
#include <iostream>
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "groupmodel.pb.h"

int main(int argc, char** argv) {
    // 初始化RPC框架
    MprpcApplication::Init(argc, argv);

    // 创建GroupService_Stub
    groupservice::GroupService_Stub stub(new MprpcChannel());

    // 创建群组
    {
        // groupservice::CreateGroupRequest request;
        // groupservice::CreateGroupResponse response;
        // request.set_groupname("Test_Group");
        // request.set_groupdesc("This_is_a_test_group.");

        // MprpcController controller;
        // stub.CreateGroup(&controller, &request, &response, nullptr);
        // if (controller.Failed()) {
        //     std::cout << "CreateGroup failed: " << controller.ErrorText() << std::endl;
        // } else {
        //     std::cout << "CreateGroup response: " << response.message() << std::endl;
        // }
    }

    // 加入群组
    {
        // groupservice::AddGroupRequest request;
        // groupservice::AddGroupResponse response;
        // request.set_userid(24);
        // request.set_groupid(7);
        // request.set_role("normal");

        // MprpcController controller;
        // stub.AddGroup(&controller, &request, &response, nullptr);
        // if (controller.Failed()) {
        //     std::cout << "AddGroup failed: " << controller.ErrorText() << std::endl;
        // } else {
        //     std::cout << "AddGroup response: " << response.message() << std::endl;
        // }
    }

    // 查询用户所在群组信息
    {
        groupservice::QueryGroupsRequest request;
        groupservice::QueryGroupsResponse response;
        request.set_userid(21);

        MprpcController controller;
        stub.QueryGroups(&controller, &request, &response, nullptr);
        if (controller.Failed()) {
            std::cout << "QueryGroups failed: " << controller.ErrorText() << std::endl;
        } else {
            std::cout << "QueryGroups response: ";
            for (const auto& group : response.groups()) {
                std::cout << "Group ID: " << group.id() << ", Group Name: " << group.groupname() << std::endl;
            }
        }
    }

    // 查询群组用户ID列表
    {
        groupservice::QueryGroupUsersRequest request;
        groupservice::QueryGroupUsersResponse response;
        request.set_userid(25);
        request.set_groupid(7);

        MprpcController controller;
        stub.QueryGroupUsers(&controller, &request, &response, nullptr);
        if (controller.Failed()) {
            std::cout << "QueryGroupUsers failed: " << controller.ErrorText() << std::endl;
        } else {
            std::cout << "QueryGroupUsers response: ";
            for (int id : response.userids()) {
                std::cout << id << " ";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}
