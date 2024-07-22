#pragma once 
#include <memory>
#include "log.hpp"
#include "Connection.hpp"
#include "Reactor.hpp"
#include "http.hpp"

CJOJ_BEGIN

// 处理 IO 事件
// recv -> http handler -> send
namespace handler
{
    // 把本轮数据读取完交给上层。
    void recv(const std::shared_ptr<Connection>& __con)
    {
        std::string __bf;
        for(;;)
        {
            ssize_t n = __con->Sock()->recv(__bf);
            if(n < 0)
            {
                if(errno == EAGAIN) // 
                    break;
                if(errno == EINTR)
                    continue;
                LOG(ERROR,"Recv error from [%s,%d] with errno(%d):%s\n",__con->Sock()->get_ip().c_str(),__con->Sock()->get_port(),errno,strerror(errno));
                return;
            }
            if(n == 0)
            {
                LOG(WARNING,"Socket[%s,%d] close\n",__con->Sock()->get_ip(),__con->Sock()->get_port());
                __con->get_reactor()->del_connection(__con);
                return;
            }
            // 将输出加入输入缓冲区
            __con->append_inbuff(__bf);
        }
        
        // 解析并执行任务
        httpServer::Serve(__con);
    }

    void send(const std::shared_ptr<Connection>& __con)
    {
        std::string __bf;
    }
}

CJOJ_END