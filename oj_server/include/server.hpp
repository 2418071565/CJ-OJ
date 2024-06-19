#pragma once

#include "init.hpp"
#include "log.hpp"
#include "util.hpp"
#include <httplib.h>
#include <jsoncpp/json/json.h>
#include <string>
#include "runner.hpp"

NAMESPACE_OJ_BEGIN

class oj_server
{
    httplib::Server Ser;
public:
    oj_server()
    {
        Ser.Get("/",[](const httplib::Request& req,httplib::Response rp)
        {
            
        });

        Ser.Post("/",[](const httplib::Request& req,httplib::Response rp)
        {
            std::string res;
            run_server(req.body,res);
            rp.set_content(res,"application/json");
        });

    }

    void daemon()
    {
        // 1. 忽略异常信号，保证守护进程不轻易退出
        signal(SIGPIPE,SIG_IGN);
        signal(SIGCHLD,SIG_IGN);
        // .... 按照需求忽略其他信号

        // 2. fork 创建子进程
        if(fork() > 0) exit(0); // 退出父进程，子进程变为孤儿进程

        // 3. 创建新会话
        setsid();

        // 4. 按照需求切换守护进程工作目录
        chdir("/");

        // 5. 处理 stdin、stdout、stderr
        // 使用 /dev/null，向该文件写入的所有数据都会被丢弃
        // 也可以直接关闭三个输入输出流，看项目需求
        int fd = open("/dev/null",O_RDWR);
        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);
    }

    void start()
    {
        Ser.listen("0.0.0.0",8888);
    }
};

NAMESPACE_OJ_END