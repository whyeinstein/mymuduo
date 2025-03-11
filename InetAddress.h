#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

// 封装socket地址类型
class InetAddress
{
public:
    // 相比muduo默认没有环回地址判断和ipv6支持
    // TODO(solved):explicit
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    // explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

    explicit InetAddress(const struct sockaddr_in &addr)
        : addr_(addr) {}

    std::string toIp() const; // TODO(solved)：const前和后
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in *getSockAddr() const { return &addr_; } // TODO(solved): 区分sockaddr_in和sockaddr
    void setSockAddr(const sockaddr_in &addr) { addr_ = addr; }

private:
    sockaddr_in addr_;
};