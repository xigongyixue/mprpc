#pragma once

#include "lockqueue.h"
#include <string>

enum LogLevel {
    INFO, // 普通信息
    ERROR, // 错误信息
};

#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0) \

#define LOG_ERR(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0) \

class Logger {
    public:
        // 单例模式
        static Logger& GetInstance();
        // 设置日志级别
        void SetLogLevel(LogLevel level);
        // 写入日志
        void Log(std::string msg);
    private:
        int log_level; // 日志级别
        LockQueue<std::string> log_queue; // 日志队列

        Logger();
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
};