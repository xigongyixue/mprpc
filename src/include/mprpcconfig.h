#pragma once

#include <unordered_map>
#include <string>

// 读取配置文件类
class MprpcConfig {
    public:
        // 解析配置文件
        void LoadConfigFile(const char *config_file);
        // 获取配置项的值
        std::string Load(const std::string &key);
    private:
        std::unordered_map<std::string, std::string> mprpc_config;
};