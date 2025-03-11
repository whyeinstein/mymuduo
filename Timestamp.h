#pragma once

#include <iostream>
#include <string>

class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch_); // TODO:为什么需要explicit,为什么认为microSecondsSinceEpoch_可以隐士转换为Timestamp

    // 获取当前时间戳
    static Timestamp now();

    // 获取时间戳的字符串表示(常方法)
    std::string toString() const;

    ~Timestamp();

private:
    int64_t microSecondsSinceEpoch_;
};
