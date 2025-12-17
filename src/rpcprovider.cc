#include "rpcprovider.h"
#include "mprpcapplication.h"

#include <string>
#include <functional>

// 发布rpc服务
void RpcProvider::NotifyService(google::protobuf::Service *service){

}

// 启动rpc服务节点
void RpcProvider::Run() {
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&event_loop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调方法 分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1)); // std::placeholders::_1 表示函数的第一个参数
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);
    
    std:: cout << "Rpc服务启动成功, 监听地址 " << ip << ":" << port << std::endl;
    // 启动服务器
    server.start();
    event_loop.loop();
}

// 新的socket连接到来时，回调此函数
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {

}

// 已建立连接用户得到读写事件，回调此函数
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp receive_time) {

}