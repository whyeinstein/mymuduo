#include "Channel.h"
#include "Logger.h"
#include "EventLoop.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false)
{
}

Channel::~Channel()
{
}

// TODO(solved)：channel的tie方法什么时候调用过？一个TcpConnection新连接创建的时候 TcpConnection => Channel
void Channel::tie(const std::shared_ptr<void> &obj)
{
    // 弱智能指针，防止connection对象提前析构后，还去调用TcpConnection的回调函数
    tie_ = obj;
    tied_ = true;
}

/**
 * 当改变channel所表示的fd的events事件后，update负责在poller中更改fd相应的事件epoll_ctl
 * Eventloop => Channelist  Poller
 */
void Channel::update()
{
    // 通过channel所属的EventLoop，调用poller的相应方法，注册fd的events事件
    // add code here
    loop_->updateChannel(this);
}

// 在channel所属的EventLoop中，把当前的channel删除掉
void Channel::remove()
{
    // add code here
    loop_->removeChannel(this);
}

// todo(solved):理解
// 对于tcpconnection的channel，因为进行可tie的绑定，handleEvent会尝试加强智能指针，确保对象在事件处理期间不会被销毁
// 对于acceptor的channel，不会进行tie的绑定，因为acceptor的channel不会被销毁
void Channel::handleEvent(Timestamp receiveTime)
{
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock(); // 确保对象在事件处理期间不会被销毁
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

// 根据poller通知的channel发生的具体事件， 由channel负责调用具体的回调操作
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revents:%d\n", revents_);

    // TODO(solved)：代表什么情况
    // 这个 if 语句的条件是：文件描述符上发生了挂起事件（EPOLLHUP），并且没有数据可读（!EPOLLIN）。
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if (closeCallback_)
        {
            closeCallback_();
        }
    }

    if ((revents_ & EPOLLERR))
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }

    // EPOLLIN 表示文件描述符上有数据可读，EPOLLPRI 表示有紧急数据可读
    if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT)
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }
}
