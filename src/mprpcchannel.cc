#include "mprpcchannel.h"

// 所有通过stub调用的rpc方法都会转发到这个函数，统一做序列化和网络传输
void MprpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method, ::google::protobuf::RpcController* controller,
                const ::google::protobuf::Message* request, ::google::protobuf::Message* response, ::google::protobuf::Closure* done) {
                    
                }