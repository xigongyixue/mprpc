#include <iostream>

#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"

int main(int argc, char *argv[]) {

    // 初始化
    MprpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法请求参数
    fixbug::LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("123456");

    // rpc方法响应
    fixbug::LoginResponse response;
    
    // 发起rpc方法调用，同步rpc调用 MprpcChannel::callmethod
    stub.Login(nullptr, &request, &response, nullptr);

    // 读取调用结果
    if(response.result().errcode() == 0){
        std::cout << "rpc login response:" << response.success() << std::endl;
    } else {
        std::cout << "rpc login response:" << response.result().errmsg() << std::endl;
    }

    return 0;
}