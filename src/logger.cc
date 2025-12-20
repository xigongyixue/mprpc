#include "logger.h"
#include <cstdio>
#include <time.h>
#include <iostream>

// 单例模式
Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    // 启动写日志线程
    std::thread writeLogTask([&](){
        while(true) {
            // 获取当前时间
            time_t now = time(nullptr);
            tm* now_tm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", now_tm->tm_year + 1900,now_tm->tm_mon + 1, now_tm->tm_mday);

            FILE *fp = fopen(file_name, "a+");
            if(fp == nullptr) {
                std::cout << "logger file : " << file_name << " open failed!" << std::endl;
                exit(-1);
            }

            std::string msg = log_queue.pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d =>[%s] ", now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec, log_level == INFO ? "INFO" : "ERROR");
            msg.insert(0, time_buf);
            msg.append("\n");
            fputs(msg.c_str(), fp);
            fclose(fp);
        }
    });
    // 分离线程
    writeLogTask.detach();
}

// 设置日志级别
void Logger::SetLogLevel(LogLevel level){
    log_level = level;
}

// 写入日志
void Logger::Log(std::string msg){
    log_queue.push(msg);
}