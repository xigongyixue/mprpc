#include "test.pb.h"
#include <bits/stdc++.h>

using namespace std;
using namespace fixbug;

int main() {

    // 封装一个登录请求
    LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("password123");

    string send_str;
    request.SerializeToString(&send_str); // 序列化
    cout << send_str.c_str() << endl;

    LoginRequest recv_request;
    recv_request.ParseFromString(send_str); // 反序列化
    cout << recv_request.name() << endl;
    cout << recv_request.pwd() << endl;


    return 0;
}

