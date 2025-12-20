#include <iostream>

#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"

int main(int argc, char *argv[]) {

    // 初始化
    MprpcApplication::Init(argc, argv);

    // 调用rpc的Login方法
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
        std::cout << "rpc login success: " << response.success() << std::endl;
    } else {
        std::cout << "rpc login error: " << response.result().errmsg() << std::endl;
    }

    // 调用rpc的Register方法
    fixbug::RegisterRequest req;
    fixbug::RegisterResponse rsp;
    req.set_id(123);
    req.set_name("mprpc");
    req.set_pwd("123456");
    stub.Register(nullptr, &req, &rsp, nullptr);

    if(rsp.result().errcode() == 0){
        std::cout << "rpc register success: " << rsp.success() << std::endl;
    } else {
        std::cout << "rpc register error: " << rsp.result().errmsg() << std::endl;
    }

    return 0;
}