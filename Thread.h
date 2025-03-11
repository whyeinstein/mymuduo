#pragma once

#include "noncopyable.h"

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc, std::string name = std::string());
    ~Thread();

    void start(); // 启动线程
    void join();  // 等待线程结束

    bool started() const { return started_; }
    pid_t tid() const { return tid_; }

    const std::string &name() const { return name_; }
    static int numCreated() { return numCreated_; }

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_; // 智能指针控制线程的生命周期，而不是让线程对象自己控制
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    static std::atomic_int numCreated_;
};