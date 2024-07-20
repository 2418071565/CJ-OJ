#pragma once
#include "socket.hpp"
#include "log.hpp"
#include "Connection.hpp"
#include "threadpool.hpp"
#include "Handler.hpp"
#include <vector>
#include <unordered_map>

// Linux
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

CJOJ_BEGIN
class CJ;


/**
 * @brief 对系统 epoll 的封装
 */
class epoller
{
    int _M_epfd;    // epoll fd
public:
    epoller()
        : _M_epfd(epoll_create(1))
    {
        if (_M_epfd < 0)
        {
            LOG(FATAL, "Epoller create error with errno:%d\n", errno);
            exit(-1);
        }
    }

    // 添加对 fd 的 events 事件的关心
    int add(int fd, uint32_t events) noexcept
    {
        epoll_event __ev;
        __ev.events = events;
        __ev.data.fd = fd;

        if (epoll_ctl(_M_epfd, EPOLL_CTL_ADD, fd, &__ev) < 0)
            return LOG(ERROR, "Add event to %d error with errno:%d\n", fd, errno), -1;
        return 0;
    }

    // 修改对 fd 关心的事件
    int modify(int fd, uint32_t events) noexcept
    {
        epoll_event __ev;
        __ev.events = events;
        __ev.data.fd = fd;

        if (epoll_ctl(_M_epfd, EPOLL_CTL_MOD, fd, &__ev) < 0)
            return LOG(ERROR, "Modify event of %d error with errno:%d", fd, errno), -1;
        return 0;
    }

    // 取消对 fd 的关心，
    int del(int fd) noexcept
    {
        epoll_event __ev;
        __ev.events = 0;    // man手册： The event argument is ignored and can be NULL (but see BUGS below).
        __ev.data.fd = fd;

        if (epoll_ctl(_M_epfd, EPOLL_CTL_DEL, fd, &__ev) < 0)
            return LOG(ERROR, "Del event of %d error with errno:%d", fd, errno), -1;
        return 0;
    }

    /**
     *  @param revents 输出型参数，返回就绪事件
     *  @param maxevents 要获取的最大就绪事件数、
     *  @param timeout 等待时间（ms），设为 -1 时表示阻塞等待。
     */
    int wait(epoll_event *revents, int maxevents, int timeout)
    {
        int n = epoll_wait(_M_epfd, revents, maxevents, timeout); // 等待事件就绪

        if (n < 0)
            return LOG(ERROR, "Epoll wait error with errno:%d\n", errno), -1;
        return n;
    }

    ~epoller()
    {
        if (_M_epfd > 0)
            ::close(_M_epfd);
    }
};

// 单 Reactor 多线程
// 内容分发，连接管理
class Reactor : public __vb
{
    std::unique_ptr<threadpool> _M_workers;     // 线程池，处理事件
    std::unique_ptr<epoller> _M_epoll;
    std::unordered_map<int, std::shared_ptr<Connection>> _M_conn;
    epoll_event _M_ready_tasks[MAX_READY_TASKS];
    bool is_running;

    // 创建守护进程
    void daemon()
    {
        signal(SIGPIPE, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);
        if (fork() > 0)
            exit(0); // 退出父进程，子进程变为孤儿进程
        setsid();
        chdir("/");
        int fd = open("/dev/null", O_RDWR);
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
    }

public:

    Reactor()
        : is_running(false)
    { }

    void init(const std::string &ip = "0.0.0.0", uint16_t port = DEFAULT_PORT, int que_len = 5) noexcept
    {
        // 创建 epoll
        _M_epoll.reset(new epoller);

        // 创建 listen socket
        auto __lis = connection_creater::createET_acceptor(ip, port, this, EPOLLIN);
        // 注册 acceptor 输入 handler
        __lis->enroll_recv([this](const std::shared_ptr<tcp_sock> &__lis_sock) {
            for(;;)
            {
                // accept 获取连接
                auto sock = __lis_sock->accept();
                if(sock->fd() < 0)
                {
                    if(errno == EAGAIN) break;
                    if(errno == EINTR) continue;
                    LOG(ERROR,"Acceptor error with errno(%d):%s\n",errno,strerror(errno));
                    return;
                }
                LOG(INFO,"Get Connection[%s,%d]\n",sock->get_ip().c_str(),sock->get_port());
                auto __con = connection_creater::createET(std::move(sock),this, EPOLLIN);
                
                __con->enroll_recv(handler::recv, std::weak_ptr<Connection>(__con));
                __con->enroll_send(handler::send, std::weak_ptr<Connection>(__con));
                this->add_connection(__con);
            }
        }, __lis->Sock());

        __lis->Sock()->listen(que_len);
        add_connection(__lis);

        LOG(INFO, "Reactor init successful\n");
        LOG(INFO, "Server start in [%s,%d]\n", __lis->Sock()->get_ip().c_str(), __lis->Sock()->get_port());
    }

    // 添加链接
    void add_connection(const std::shared_ptr<Connection> &__con) noexcept
    {
        _M_epoll->add(__con->Sock()->fd(), __con->get_events());    // 添加连接到 epoller 中
        _M_conn[__con->Sock()->fd()] = __con;
    }

    // 删除连接
    void del_connection(const std::shared_ptr<Connection> &__con) noexcept
    {
        _M_epoll->del(__con->Sock()->fd()); // 从 epoller 中删除对应连接
        _M_conn.erase(_M_conn.find(__con->Sock()->fd()));
    }

    std::unique_ptr<threadpool>& get_workers() noexcept { return _M_workers; }
    std::shared_ptr<Connection> &get_connection(int fd) noexcept { return _M_conn[fd]; }
    virtual std::unique_ptr<epoller>& get_epoll() noexcept { return _M_epoll; }

    // 任务分发
    void dispatch()
    {
        int n = _M_epoll->wait(_M_ready_tasks, MAX_READY_TASKS, TIME_OUT);
        if (n == 0)
            return LOG(INFO, "Time out\n"), void();
        if (n < 0)
            return LOG(FATAL, "Epoller wait error\n"), void();
        for (int i = 0; i < n; ++i)
        {
            int fd = _M_ready_tasks[i].data.fd;
            uint32_t events = _M_ready_tasks[i].events;
            auto& con = _M_conn[fd];
            if (events & EPOLLIN)
                _M_workers->add_task(&con->recv,con.get());
            if (events & EPOLLOUT)
                _M_workers->add_task(&con->send,con.get());
        }
    }

    // 启动 IO
    void start() noexcept(false)
    {
        is_running = true;
        while (is_running)
        {
            dispatch();
        }
    }
    void stop() noexcept { is_running = false; }
};

CJOJ_END