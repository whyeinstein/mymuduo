#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;

// muduo库中多路事件分法器的核心IO复用模块
class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default; // TODO:为什么是虚函数,为什么要加 "=default"

    // 给所有的IO复用保留统一的接口，以ChannelList填充活动的通道
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    // 判断channel是否在当前Poller中
    bool hasChannel(Channel *channel) const;

    /**
     * TODO:记笔记
     * 该函数不放在Poller.cc源文件实现的原因：
     * newDefaultPoller一定会生成一个具体的I/O复用对象，并返回基类指针
     * 但是基类不应该引用派生类的头文件PollPoller.h/EpollPoller.h
     * 单独放在一个源文件实现
     */
    // EventLoop可以通过该接口获取默认的IO复用的具体实现(类似单例模式)
    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    // map的key：sockfd  value：sockfd所属的channel通道类型
    using ChannelMap = std::unordered_map<int, Channel *>; // TODO:与有序的区别
    ChannelMap channels_;                                  // 文件描述符和通道的映射表
private:
    EventLoop *ownerLoop_; // Poller所属的EventLoop循环
};