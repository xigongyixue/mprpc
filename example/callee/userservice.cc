#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

using namespace fixbug;

// rpc服务发布端
class UserService : public UserServiceRpc {
    public:
        bool Login(std::string name, std::string pwd) {
            std::cout << "Login: " << name << ", " << pwd << std::endl;
            return true;
        }

        bool Register(uint32_t id, std::string name, std::string pwd) {
            std::cout << "Register: " << id << ", " << name << ", " << pwd << std::endl;
            return true;
        }

        // 重写UserServiceRpc虚函数
        // 1. caller -> Login(LoginRequest) -> muduo -> callee
        // 2. caller -> Login(LoginRequest) -> Login() 下面这个函数
        void Login(::google::protobuf::RpcController* controller, const ::fixbug::LoginRequest* request, ::fixbug::LoginResponse* response, ::google::protobuf::Closure* done) {
            // 获取请求参数
            std::string name = request->name();
            std::string pwd = request->pwd();

            // 做本地业务
            bool login_result = Login(name, pwd);

            // 写入响应
            ResultCode *code = response->mutable_result();
            code->set_errcode(0);
            code->set_errmsg("");
            response->set_success(login_result);

            // 执行回调
            done->Run();
        }

        void Register(::google::protobuf::RpcController* controller, const ::fixbug::RegisterRequest* request, ::fixbug::RegisterResponse* response, ::google::protobuf::Closure* done) {
            uint32_t id = request->id();
            std::string name = request->name();
            std::string pwd = request->pwd();

            bool register_result = Register(id, name, pwd);

            ResultCode *code = response->mutable_result();
            code->set_errcode(0);
            code->set_errmsg("");
            response->set_success(register_result);

            done->Run();
        }
};

int main(int argc, char **argv) {
    // 初始化
    MprpcApplication::Init(argc, argv);

    // 发布服务
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 运行服务，进程进入阻塞状态，等待远程的rpc调用
    provider.Run();

}