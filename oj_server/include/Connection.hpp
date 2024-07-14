#pragma once
#include <memory>
#include <functional>
#include <sstream>
#include <sys/epoll.h>
#include "socket.hpp"

CJOJ_BEGIN

class Connection;

class __vb
{
public:
virtual void add_connection(const std::shared_ptr<Connection> &__con) = 0;
virtual void del_connection(const std::shared_ptr<Connection> &__con) = 0;
virtual std::unique_ptr<threadpool>& get_workers() = 0;
};
class Reactor ;

class Connection
{
    std::function<void()> _M_recv, _M_send;
    std::shared_ptr<tcp_sock> _M_sock;
    mutable std::stringstream _M_inbuff     // 输入缓冲区
                            , _M_outbuff;   // 输出缓冲区
    uint32_t _M_events;
    mutable __vb *_M_reactor;

public:
    Connection(const std::shared_ptr<tcp_sock> &sock, __vb *reactor, uint32_t events) noexcept
        : _M_sock(sock), _M_reactor(reactor), _M_events(events)
    { }

    // 注册输入函数
    template <class Callable, class... Args>
    void enroll_recv(Callable &&F, Args &&...args) noexcept
    {
        auto __f = std::bind(std::forward<Callable>(F), std::forward<Args>(args)...);
        _M_recv = std::move(__f);
    }

    // 注册输出函数
    template <class Callable, class... Args>
    void enroll_send(Callable &&F, Args &&...args) noexcept
    {
        auto __f = std::bind(std::forward<Callable>(F), std::forward<Args>(args)...);
        _M_send = std::move(__f);
    }

    // 向输入缓冲区写入数据
    void append_inbuff(const std::string &__s) const noexcept { _M_inbuff << __s; }
    // 向输出缓冲区写入数据
    void append_outbuff(const std::string &__s) const noexcept { _M_outbuff << __s; }
    // 判断输出缓存区是否为空
    bool outbuff_empty() noexcept { return _M_outbuff.str().empty(); }


    void recv() { _M_recv(); }
    void send() { _M_send(); }
    const std::shared_ptr<tcp_sock> &get_sock() const noexcept  { return _M_sock; }
    __vb *get_reactor() const noexcept                          { return _M_reactor; }
    u_int32_t get_events() const noexcept                       { return _M_events; }
};


// 工厂模式创建连接
class connection_creater
{
public:
    // 创建一个 ET 模式（文件描述符为非阻塞模式）的绑定到对应 ip:port 的 Connection。
    static std::shared_ptr<Connection> createET_acceptor(const std::string &ip, uint16_t port, __vb *reactor, uint32_t events)
    {
        std::shared_ptr<Connection> __con(new Connection(std::make_shared<tcp_sock>(), reactor, events | EPOLLET));
        __con->get_sock()->noblock();
        int val = 1;
        __con->get_sock()->set_opt(SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
        __con->get_sock()->bind(ip, port);
        return __con;
    }

    // 用一个已有的 socket 创建一个 ET 模式（文件描述符为非阻塞模式）的 Connection。
    static std::shared_ptr<Connection> createET(const std::shared_ptr<tcp_sock> &sock, __vb *reactor, u_int32_t events)
    {
        std::shared_ptr<Connection> __con(new Connection(sock, reactor, events | EPOLLET));
        __con->get_sock()->noblock();
        return __con;
    }
};
CJOJ_END