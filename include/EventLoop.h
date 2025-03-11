#pragma once

#include <functional>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <memory>
#include <mutex>

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

class Channel;
class Poller;

// 事件循环类 主要包含了两个大模块 Channel Poller（epoll的抽象）
class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>; // 回调函数类型

    EventLoop();
    ~EventLoop();

    void loop(); // 开启事件循环
    void quit(); // 退出事件循环

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    void runInLoop(Functor cb);   // 在当前loop中执行cb
    void queueInLoop(Functor cb); // 把cb放入队列中，唤醒loop所在的线程，执行cb

    void wakeup(); // 用来唤醒loop所在的线程的

    void updateChannel(Channel *channel); // EventLoop的方法 => Poller的方法
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); } // 判断EventLoop对象是否在自己的线程里面

private:
    void handleRead();        // wake up
    void doPendingFunctors(); // 执行回调

    using ChannelList = std::vector<Channel *>;

    std::atomic_bool looping_; // 原子操作，通过CAS实现
    std::atomic_bool quit_;    // 标识退出loop循环

    const pid_t threadId_; // 当前loop所在线程的ID

    Timestamp pollReturnTime_;       // poller返回发生事件的Channels的时间点
    std::unique_ptr<Poller> poller_; // Poller对象

    /*
     * TODO：eventfd,socketpair
     */
    int wakeupFd_; // 主要作用，当mainLoop获取一个新用户的channel，通过轮询算法选择一个subloop，通过该成员唤醒subloop处理channel
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    // Channel *currentActiveChannel_;

    std::atomic_bool callingPendingFunctors_; // 标识当前loop是否有需要执行的回调函数
    std::vector<Functor> pendingFunctors_;    // 存储loop需要执行的所有的回调操作
    std::mutex mutex_;                        // 互斥锁，用来保护上面vector容器的线程安全操作
};
