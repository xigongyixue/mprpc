#include "mprpcapplication.h"
#include <iostream>
#include "unistd.h"
#include <string>

MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp() {
    std::cout << "Usage: command -i <config_file>" << std::endl;
}

void MprpcApplication::Init(int argc, char **argv) {
    // 读取配置文件路径
    if (argc < 2) {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1) {
        if(c == 'i') {
            config_file = optarg;
        } else if (c == '?') {
            std::cout << "invalid args!" << std::endl;
            ShowArgsHelp();
        } else if (c == ':') {
            std::cout << "need args!" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        }
    }
    m_config.LoadConfigFile(config_file.c_str());
}

MprpcApplication& MprpcApplication::GetInstance() {
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig() {
    return m_config;
}