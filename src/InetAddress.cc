#include "InetAddress.h"

#include <strings.h>
#include <string.h>
#include <iostream>

InetAddress::InetAddress(uint16_t port, std::string ip) // TODO(solved): 理解这一步在干嘛
{
    bzero(&addr_, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

std::string InetAddress::toIp() const // TODO(solved): 区分两种方法
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    return buf;
    // return std::string(inet_ntoa(addr_.sin_addr));
}

std::string InetAddress::toIpPort() const
{
    // ip:port
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    size_t end = strlen(buf);
    uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf + end, ":%u", port); // TODO(solved): snprintf和sprintf的区别
    return buf;
}

uint16_t InetAddress::toPort() const
{
    // 注意与inet_ntoa/p的区别
    return ntohs(addr_.sin_port); // TODO(solved)：ntohs和ntohl的区别
}

// int main()
// {
//     InetAddress addr(8080);
//     std::cout << addr.toIpPort() << std::endl;
//     return 0;
// }