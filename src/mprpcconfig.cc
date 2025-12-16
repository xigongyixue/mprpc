#include "mprpcconfig.h"

#include <algorithm>
#include <iostream>
#include <string>

// 解析配置文件
void MprpcConfig::LoadConfigFile(const char *config_file) {
    FILE *fp = fopen(config_file, "r");
    if (fp == NULL) {
        std::cout << config_file << "配置文件打开失败" << std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(fp)) {
        char buf[1024] = {0};
        fgets(buf, sizeof(buf), fp);

        // 去掉字符串前后多余的空格
        std::string src_buf(buf);
        int idx = src_buf.find_first_not_of(' ');
        if(idx != -1) {
            src_buf = src_buf.substr(idx, src_buf.size() - idx);
        }
        idx = src_buf.find_last_not_of(' ');
        if(idx != -1) {
            src_buf = src_buf.substr(0, idx + 1);
        }

        // 判断#注释
        if(src_buf.empty() || src_buf[0] == '#') {
            continue;
        }

        // 解析配置项
        idx = src_buf.find('=');
        if(idx == -1) {
            continue;
        }
        std::string key = src_buf.substr(0, idx);
        std::string value = src_buf.substr(idx + 1);
        value.pop_back(); //去掉最后一个回车符
        mprpc_config[key] = value;
    }
    
}
// 获取配置项的值
std::string MprpcConfig::Load(const std::string &key) {
    if(mprpc_config.find(key) != mprpc_config.end()) {
        return mprpc_config[key];
    }
    return "";
}