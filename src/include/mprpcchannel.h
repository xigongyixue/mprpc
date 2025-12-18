#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

class MprpcChannel : public ::google::protobuf::RpcChannel{
public:
    // 所有通过stub调用的rpc方法都会转发到这个函数，统一做序列化和网络传输
    void CallMethod(const ::google::protobuf::MethodDescriptor* method, ::google::protobuf::RpcController* controller,
                   const ::google::protobuf::Message* request, ::google::protobuf::Message* response, ::google::protobuf::Closure* done) override;

};
