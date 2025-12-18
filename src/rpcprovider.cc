#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"

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
    if(!conn->connected()) {
        // 与rpc client断开连接
        conn->shutdown();
    }
}

/*
content: service_name method_name args
format: header_size(4B)+header_str+args_str
*/

// 已建立连接用户得到读写事件，回调此函数
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp receive_time) {
    // 网络上接受的远程rpc调用请求的字节流
    std::string recv_buf = buf->retrieveAllAsString();

    // 拷贝4个字节到header_size变量中
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 读取数据头的原始字符流
    std::string rpc_header_str = recv_buf.substr(4, header_size); // service_name + method_name + args_size
    mprpc::RpcHeader rpc_header;
    std::string service_name, method_name;
    uint32_t args_size;
    if(rpc_header.ParseFromString(rpc_header_str)) { // 反序列化
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        args_size = rpc_header.args_size();
    } else {
        std::cout << "rpc header str: " << rpc_header_str << " parse error!" << std::endl;
        return;
    }

    // 读取参数的原始字符流
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印
    std::cout << "=====================================================" << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "=====================================================" << std::endl;

    // 获取service对象和method对象
    if(services.find(service_name) == services.end()) {
        std::cout << service_name << "service not found!" << std::endl;
        return;
    }
    
    if(services[service_name].methods.find(method_name) == services[service_name].methods.end()) {
        std::cout << service_name << ":" << method_name << "method not found!" << std::endl;
        return;
    }

    google::protobuf::Service *service = services[service_name].service;
    const google::protobuf::MethodDescriptor *method = services[service_name].methods[method_name];

    // 生成rpc方法调用的请求和响应
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)) {
        std::cout << "request parse error, content:" << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 调用rpc方法
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                                                    (this, &RpcProvider::SendRpcResponse, conn, response);
    service->CallMethod(method, NULL, request, response, done);

}

 // 发送rpc响应
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    if(response->SerializeToString(&response_str)) { // 序列化
        conn->send(response_str);
    } else {
        std::cout << "response serialize error!" << std::endl;
    }
    conn->shutdown(); // 主动断开连接
}