/*
 * @version: 
 * @Author: zsq 1363759476@qq.com
 * @Date: 2024-07-23 10:18:03
 * @LastEditors: zsq 1363759476@qq.com
 * @LastEditTime: 2024-07-23 10:20:18
 * @FilePath: /Linux_nc/mpRPC/mympRPC/example/calleegroupmodel/testgroupmodel.cpp
 * @Descripttion: 
 */
#include "groupmodel.pb.h"
#include "groupmodel_z.hpp"
#include "mprpcapplication.h"
#include "rpcprovider.h"

class GroupServiceImpl : public groupservice::GroupService {
public:
    void CreateGroup(::google::protobuf::RpcController* controller,
                     const ::groupservice::CreateGroupRequest* request,
                     ::groupservice::CreateGroupResponse* response,
                     ::google::protobuf::Closure* done) override {
        Group group;
        group.setName(request->groupname());
        group.setDesc(request->groupdesc());

        GroupModel model;
        bool success = model.createGroup(group);

        response->set_success(success);
        response->set_message(success ? "Create group successful" : "Create group failed");
        if (success) {
            response->set_groupid(group.getId());
        }
        done->Run();
    }

    void AddGroup(::google::protobuf::RpcController* controller,
                  const ::groupservice::AddGroupRequest* request,
                  ::groupservice::AddGroupResponse* response,
                  ::google::protobuf::Closure* done) override {
        GroupModel model;
        model.addGroup(request->userid(), request->groupid(), request->role());

        response->set_success(true);
        response->set_message("Add group successful");
        done->Run();
    }

    void QueryGroups(::google::protobuf::RpcController* controller,
                     const ::groupservice::QueryGroupsRequest* request,
                     ::groupservice::QueryGroupsResponse* response,
                     ::google::protobuf::Closure* done) override {
        GroupModel model;
        std::vector<Group> groups = model.queryGroups(request->userid());

        for (auto& group : groups) {
            groupservice::Group* grp = response->add_groups();
            grp->set_id(group.getId());
            grp->set_groupname(group.getName());
            grp->set_groupdesc(group.getDesc());

            for (auto& user : group.getUsers()) {
                groupservice::GroupUser* grp_user = grp->add_users();
                grp_user->set_id(user.getId());
                grp_user->set_name(user.getName());
                grp_user->set_state(user.getState());
                grp_user->set_role(user.getRole());
            }
        }
        done->Run();
    }

    void QueryGroupUsers(::google::protobuf::RpcController* controller,
                         const ::groupservice::QueryGroupUsersRequest* request,
                         ::groupservice::QueryGroupUsersResponse* response,
                         ::google::protobuf::Closure* done) override {
        GroupModel model;
        std::vector<int> userids = model.queryGroupUsers(request->userid(), request->groupid());

        for (int id : userids) {
            response->add_userids(id);
        }
        done->Run();
    }
};

int main(int argc, char** argv) {
    // 初始化RPC框架
    MprpcApplication::Init(argc, argv);

    // 创建并注册GroupService服务
    RpcProvider provider;
    provider.NotifyService(new GroupServiceImpl());

    // 启动RPC服务发布节点
    provider.Run();

    return 0;
}
