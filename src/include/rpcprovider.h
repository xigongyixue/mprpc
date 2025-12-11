#pragma once
#include "google/protobuf/service.h"

// 框架提供的专门服务发布rpc服务的网络对象类
class RpcProvider {
    public:
        // 发布rpc服务
        void NotifyService(google::protobuf::Service *service);

        // 启动rpc服务节点
        void Run() {}
};