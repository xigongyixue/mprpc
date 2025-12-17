#include "rpcprovider.h"
#include "mprpcapplication.h"

// service_name -> serviceInfo <service对象, {<method_name,method方法对象>···}

// 发布rpc服务
void RpcProvider::NotifyService(google::protobuf::Service *service){

    ServiceInfo service_info;

    // 获取服务对象描述信息
    const google::protobuf::ServiceDescriptor* service_descriptor = service->GetDescriptor();
    // 获取服务名字
    std::string server_name = service_descriptor->name();
    // 获取服务对象方法数量
    int method_count = service_descriptor->method_count();

    std::cout << "service_name: " << server_name << std::endl;

    for(int i = 0; i < method_count; i++) {
        // 获取服务对象指定下标服务方法描述信息
        const google::protobuf::MethodDescriptor* method_descriptor = service_descriptor->method(i);
        std::string method_name = method_descriptor->name();
        service_info.methods.insert({method_name, method_descriptor});

        std::cout << "method_name: " << method_name << std::endl;
    }
    service_info.service = service;
    services.insert({server_name, service_info});
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
    
    std:: cout << "Rpc services is listening on " << ip << ":" << port << std::endl;
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