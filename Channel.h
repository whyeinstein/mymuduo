#pragma once

#include <functional>
#include <memory>

#include "Timestamp.h"
#include "noncopyable.h"

// 前置声明即可（具体实现在源文件中，不用引入相关头文件，暴露信息更少）
class EventLoop;

/**
 * 理清楚 EventLoop Channel Poller之间的关系    《= Reactor模型上对应 Demultiplexer
 * Channel理解为通道， 封装了sockfd和其感兴趣的事件event,如EPOLLIN、EPOLLOUT等
 * 还绑定了poller返回的具体事件
 */

class Channel : noncopyable
{
public:
    typedef std::function<void()> EventCallback; // 事件回调
    typedef std::function<void(Timestamp)> ReadEventCallback;
    // TODO(solved):C++11特性？？？,两种方法区别
    // using EventCallback = std::function<void()>;
    // using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    // fd得到poller通知以后处理事件，进行相应的回调操作
    void handleEvent(Timestamp receiveTime); // 形参为事件发生的时间戳，需要引入头文件，不能仅声明

    // 设置回调函数对象
    // 函数对象的赋值操作(将cb转化为右值)
    void setReadCallback(const ReadEventCallback &cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(const EventCallback &cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(const EventCallback &cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(const EventCallback &cb) { errorCallback_ = std::move(cb); }

    void tie(const std::shared_ptr<void> &); // 防止channel被手动remove掉，channel还在执行回调操作
    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }

    // 设置fd相应的事件状态
    void enableReading()
    {
        events_ |= kReadEvent;
        update();
    }
    void disableReading()
    {
        events_ &= ~kReadEvent;
        update();
    }
    void enableWriting()
    {
        events_ |= kWriteEvent;
        update();
    }
    void disableWriting()
    {
        events_ &= ~kWriteEvent;
        update();
    }
    void disableAll()
    {
        events_ = kNoneEvent;
        update();
    }

    // 返回fd对应的事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isReading() const { return events_ == kReadEvent; }
    bool isWriting() const { return events_ == kWriteEvent; }

    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    // one loop per thread, 返回当前channel所属的EventLoop
    // 一个EventLoop只属于一个线程,但可以有多个channel,只有一个poller
    EventLoop *ownerLoop() { return loop_; }

    // 在channel所属的EventLoop中，把当前的channel删除掉
    void remove();

private:
    void update();                                    // 更新channel所属的EventLoop中的poller监听的fd的events事件
    void handleEventWithGuard(Timestamp receiveTime); // 受保护的事件处理函数，由handleEvent调用

    static const int kNoneEvent;  // 无事件
    static const int kReadEvent;  // fd对读事件感兴趣
    static const int kWriteEvent; // fd对写事件感兴趣

    EventLoop *loop_; // 事件循环
    const int fd_;    // fd，Poller监听的对象
    int events_;      // 注册fd关心的事件
    int revents_;     // Poller返回的具体发生事件
    int index_;       // 详见EPollPoller.cc全局变量

    std::weak_ptr<void> tie_;
    bool tied_;

    // 因为channel通道里能够获知fd最终发生的具体的事件revents_，所以它负责调用具体事件的回调操作
    ReadEventCallback readCallback_; // 读事件回调
    EventCallback writeCallback_;    // 写事件回调
    EventCallback closeCallback_;    // 关闭事件回调
    EventCallback errorCallback_;    // 错误事件回调
};
