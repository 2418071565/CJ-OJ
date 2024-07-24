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
            LOG(DEGUB,"Recv:\n%s\n",__bf.c_str());
            if(n < 0)
            {
                if(errno == EAGAIN) // 
                    break;
                if(errno == EINTR)
                    continue;
                LOG(ERROR,"Recv from [%s,%d] error with errno:%s\n",__con->Sock()->get_ip().c_str(),__con->Sock()->get_port(),strerror(errno));
                return;
            }
            if(n == 0)
            {
                LOG(INFO,"Socket[%s,%d] close\n",__con->Sock()->get_ip().c_str(),__con->Sock()->get_port());
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
        std::stringstream& obf = __con->outBuffer();
        char buff[2048];
        for(;!obf.eof();)
        {
            obf.read(buff,sizeof(buff));

            ssize_t n = __con->Sock()->send(buff,obf.gcount());
            LOG(DEGUB,"send:\n%s\n",buff);
            if(n > 0)
                continue;
            else if(n == 0)
            {
                LOG(INFO,"Connection:[%s:%d] close socket\n",__con->Sock()->get_ip().c_str(),__con->Sock()->get_port());
                __con->get_reactor()->del_connection(__con);
                return;
            }
            else 
            {
                if(errno == EAGAIN)
                    break;
                if(errno == EINTR)
                    continue;
                LOG(ERROR,"Send erorr to [%s:%d] wiht errno:%s\n",__con->Sock()->get_ip().c_str(),__con->Sock()->get_port(),strerror(errno));
                __con->get_reactor()->del_connection(__con);
                return;
            }
        }

        if(obf.eof())
            __con->enableWrite(false);
        else __con->enableWrite(true);
    }
}

CJOJ_END