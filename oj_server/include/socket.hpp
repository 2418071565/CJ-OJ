#pragma once
#include <string>
#include <cstring>
#include <memory>
#include "log.hpp"
#include <sys/types.h>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// Linux
#include <unistd.h>
#include <fcntl.h>

CJOJ_BEGIN

// 网络套接字
class sock
{
protected:
    int _M_fd = -1;     // 套接字 fd

    // 套接字绑定的 IP:Port
    // 主机字节序的 port
    uint16_t _M_port;  
    std::string _M_ip;  

    // 通过 IP:Port 创建一个 sockaddr_in 类型 socket 地址
    virtual void create_sockaddr_in(const std::string& ip,uint16_t port,sockaddr_in& addr /*输出型参数*/ ) = 0;
public:

    sock(int domain,int type,int protocol) noexcept(false)
        :_M_fd(socket(domain,type,protocol)) 
    { 
        if(_M_fd < 0)
        {
            LOG(ERROR,"socket error with errno:%d",errno);
            throw "socket error";
        }
    }

    // 通过 fd 创建 socket
    explicit sock(int sockfd)
        :_M_fd(sockfd)
    { }

    // 设置 socket 选项
    int set_opt(int level, int optname, const void *optval, socklen_t optlen) noexcept
    {
        if(::setsockopt(_M_fd,level,optname,optval,optlen) < 0) 
            return LOG(WARNING,"setsockopt error with errno:%d",errno),0;
        return -1;
    }

    // 获取 socket 选项
    int get_opt(int level, int optname,void *optval, socklen_t *optlen) noexcept
    {
        if(::getsockopt(_M_fd,level,optname,optval,optlen) < 0)
            return LOG(WARNING,"getsockopt error with errno:%d",errno),0;
        return -1;
    }

    int get_fd() const noexcept                 { return _M_fd;}
    uint16_t get_port() const noexcept          { return _M_port; }
    const std::string& get_ip() const noexcept  { return _M_ip; }
    void close() { ::close(_M_fd); }

    // 设置 socket 为非阻塞模式
    int noblock() noexcept
    {
        int f1 = fcntl(_M_fd,F_GETFL);
        if(f1 < 0)
            return LOG(WARNING,"Set noblock failed fd:%d\n",_M_fd),-1;
        return fcntl(_M_fd,F_SETFL, f1 | O_NONBLOCK),0;
    }

    virtual int bind(const std::string& ip,uint32_t port) = 0;
    virtual ssize_t send(const std::string&,int) = 0;
    virtual ssize_t recv(std::string&,int) = 0;
    virtual ~sock() { close(); }
};

// 封装 tcp socket 系统调用
// 维护 socketfd ，ip，port 
class tcp_sock: public sock
{
    // ip + port 创建 sockaddr_in
    void create_sockaddr_in(const std::string& ip,uint16_t port,sockaddr_in& addr/* 输出型参数 */) noexcept
    {
        memset(&addr,0,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET,ip.c_str(),static_cast<void*>(&addr.sin_addr.s_addr));
    }
public:
    tcp_sock()
        : sock(AF_INET,SOCK_STREAM,IPPROTO_TCP)
    { }

    tcp_sock(int fd)
        : sock(fd)
    { }

    // 绑定 ip：port，成功返回 0，失败返回 -1，errno 被设置
    // 主机字节序的 port
    virtual int bind(const std::string& ip,uint32_t port) noexcept
    {
        // 创建 ip + port 地址
        sockaddr_in _locl;
        create_sockaddr_in(ip,port,_locl);
        if(::bind(_M_fd,(sockaddr*)&_locl,sizeof(sockaddr_in)) < 0) // 绑定
        {
            LOG(WARNING,"bind error with errno:%d",errno);
            return -1;
        }
        _M_ip = ip,_M_port = port;
        return 0;
    }

    
    virtual ssize_t send(const std::string& send_str,int flags = 0)
    {
        return ::send(_M_fd,send_str.c_str(),send_str.size(),flags);
    }


    virtual ssize_t recv(std::string& ret_str,int flags = 0)
    {
        char buff[2048];
        ssize_t n = ::recv(_M_fd,buff,sizeof(buff) - 1,flags);
        if(n <= 0)return n;
        buff[n] = 0;
        ret_str = buff;
        return n;
    }

    virtual ssize_t recv(char* buff,size_t buff_size,int flags = 0)
    {
        ssize_t n = ::recv(_M_fd,buff,buff_size,flags);
        if(n <= 0)return n;
        return n;
    }

    // 开始监听端口，成功返回 0，失败返回 -1，errno 被设置
    int listen(int que_len = 5) noexcept
    {
        if(::listen(_M_fd,que_len) < 0)
        {
            LOG(ERROR,"Server listen error with errno:%d\n",errno);
            return -1;
        }
        return 0;
    }

    // 从全连接队列获取 tcp 连接
    std::unique_ptr<tcp_sock> accept() noexcept
    {
        sockaddr_in _client;
        socklen_t _len = sizeof(sockaddr_in);
        memset(&_client,0,sizeof(_client));

        int nfd = ::accept(_M_fd,(sockaddr*)&_client,&_len);
        if(nfd < 0)
            return std::unique_ptr<tcp_sock>(new tcp_sock(-1));
        // 创建 tcp socket
        std::unique_ptr<tcp_sock> sptr(new tcp_sock(nfd));
        sptr->_M_port = ntohs(_client.sin_port);
        char buff[128] = { 0 };
        inet_ntop(AF_INET,&_client.sin_addr.s_addr,buff,sizeof(sockaddr_in));
        sptr->_M_ip = buff;
        return sptr;
    }

    // 连接指定 ip:port 成功返回 0，失败返回 -1，errno 被设置。
    int connect(const std::string& ip,uint32_t port) noexcept
    {
        sockaddr_in _server;
        create_sockaddr_in(ip,port,_server);

        if(::connect(_M_fd,(sockaddr*)&_server,sizeof(sockaddr_in)) < 0)
        {
            LOG(WARNING,"connect error with errno:%d",errno);
            return -1;
        }
        return 0;
    }
};

// UDP socket 之后拓展
class udp_sock : public sock
{

};

CJOJ_END

