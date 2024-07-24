#pragma once
#include "log.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>


CJOJ_BEGIN
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
            return LOG(ERROR, "Epoll wait error with errno:%d  %s\n", errno,strerror(errno)), -1;
        return n;
    }

    ~epoller()
    {
        if (_M_epfd > 0)
            ::close(_M_epfd);
    }
};

CJOJ_END