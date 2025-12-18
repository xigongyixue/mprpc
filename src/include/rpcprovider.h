#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>


// 框架提供的专门服务发布rpc服务的网络对象类
class RpcProvider {
    public:
        // 发布rpc服务
        void NotifyService(google::protobuf::Service *service);

        // 启动rpc服务节点
        void Run();
    private:
        muduo::net::EventLoop event_loop;

        // 服务类型信息
        struct ServiceInfo {
            google::protobuf::Service *service; // 服务对象
            std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> methods; // 服务方法
        };

        // 注册成功的服务对象和其方法信息
        std::unordered_map<std::string, ServiceInfo> services; 

        // 新的socket连接到来时，回调此函数
        void OnConnection(const muduo::net::TcpConnectionPtr& conn);

        // 已建立连接用户得到读写事件，回调此函数
        void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp receive_time);

        // 发送rpc响应
        void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);
};