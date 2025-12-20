#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include "mprpcapplication.h"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <error.h>

// header_size(4B) + service_name + method_name + rargs_size + args
// 所有通过stub调用的rpc方法都会转发到这个函数，统一做序列化和网络传输
void MprpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method, ::google::protobuf::RpcController* controller,
                const ::google::protobuf::Message* request, ::google::protobuf::Message* response, ::google::protobuf::Closure* done) {
    const google::protobuf::ServiceDescriptor* service = method->service();
    std::string service_name = service->name();
    std::string method_name = method->name();
    uint32_t args_size = 0;
    std::string args_str;

    // 获取请求参数长度
    if(request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        std::cout << "serialize request failed" << std::endl;
        return;
    }

    // 组装请求头
    mprpc::RpcHeader rpcHeader;
    uint32_t header_size = 0;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);
    std::string header_str;
    if(!rpcHeader.SerializeToString(&header_str)) {
        std::cout << "serialize rpcHeader failed" << std::endl;
        return;
    } else {
        header_size = header_str.size();
    }

    // 组织要发送的字符串
    std::string send_rpc_str = std::string((char*)&header_size, 4) + header_str + args_str;

    // 打印
    std::cout << "=====================================================" << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "=====================================================" << std::endl;

    // 使用tcp完成rpc调用
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd == -1) {
        std::cout << "create socket error! errno:" << errno << std::endl;
        return;
    }
 
    // 读取配置文件
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    // 连接rpc服务器
    if(connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cout << "connect error! errno:" << errno << std::endl;
        close(client_fd);
        return;
    }

    // 发送数据
    if(send(client_fd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
        std::cout << "send error! errno:" << errno << std::endl;
        return;
    }

    // 接收数据
    char buf[1024] = {0};
    int recv_size = 0;
    if( (recv_size = recv(client_fd, buf, sizeof(buf), 0)) == -1) {
        std::cout << "recv error! errno:" << errno << std::endl;
        close(client_fd);
        return;
    }

    if(!response->ParseFromArray(buf, recv_size)) {
        std::cout << "parse response failed!" << std::endl;
        close(client_fd);
        return;
    }
}