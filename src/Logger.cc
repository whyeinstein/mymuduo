#include <iostream>

#include "Logger.h"
#include "Timestamp.h"

// 获取日志唯一的实例对象
Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::setLogLevel(int level)
{
    logLevel_ = level;
}

// 写日志 [级别信息] time : message
void Logger::log(std::string message)
{
    switch (logLevel_)
    {
    case INFO:
        std::cout << "[INFO]";
        break;
    case ERROR:
        std::cout << "[ERROR]";
        break;
    case FATAL:
        std::cout << "[FATAL]";
        break;
    case DEBUG:
        std::cout << "[DEBUG]";
        break;
    default:
        break;
    }

    // 打印时间和msg, 暂时先使用muduo库自带的时间函数
    std::cout << Timestamp::now().toString() << " : " << message << std::endl;
}