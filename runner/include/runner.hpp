#pragma once
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <jsoncpp/json.h>
#include "init.hpp"
#include "util.hpp"
#include "log.hpp"


NAMESPACE_OJ_BEGIN


/** @brief 执行编译服务，g++ -o {file_name} 
 *  
 *  @param file_name 不带后缀的文件名
*/
bool compile(const std::string& file_name)
{
    int _ch_id = fork();
    if(_ch_id < 0)
    {
        log(ERROR) << "Compiler fork error." << std::endl;
        exit(-1);
    }

    // 生存文件路径
    std::string _src_path = path::get_src(file_name);
    std::string _comp_err_path = path::get_comp_err(file_name);
    std::string _elf_path = path::get_elf(file_name);
    

    // 子进程
    // 编译源文件，如果出错，要输出错误信息到对应文件中
    if(_ch_id == 0) 
    {   
        umask(0);
        // 将编译错误输出到文件中
        int _fd = open(_comp_err_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,0664);
        if(_fd < 0)
        {
            log(ERROR) << "Open error file failed." << std::endl;
            exit(0);
        }
        dup2(_fd,2);

        // 替换 g++
        execlp("g++","g++","-o",_elf_path.c_str(),_src_path.c_str(),"-std=c++17",nullptr /* 参数结束 */);


        // 替换失败
        log(ERROR) << "Exec g++ error." << std::endl;
        exit(0);
    }

    // 父进程
    waitpid(_ch_id,nullptr,0); // 阻塞等待


    // 检查是否生成可执行文件
    if(std::filesystem::exists(_elf_path))
    {
        log(INFO) << "Compile successfully.\n";
        return true;
    }
    log(WARNING) << "Compile file error: " + std::string(_src_path) << std::endl;
    return false;
}

using namespace util;


// 设置用户程序运行资源限制：时间和内存(KB)
bool set_limit(int time_limit,int mem_limit)
{
    rlimit time;
    time.rlim_cur = time_limit;
    time.rlim_max = RLIM_INFINITY;
    if(setrlimit(RLIMIT_CPU,&time) < 0)
    {
        log(ERROR) << "Set time limits error." << std::endl;
        return false;
    }

    rlimit mem;
    mem.rlim_cur = mem_limit * 1024; // KB -> B
    mem.rlim_max = RLIM_INFINITY;
    if(setrlimit(RLIMIT_DATA,&mem) < 0)
    {
        log(ERROR) << "Set memory limits error." << std::endl;
        return false;
    }
    return true;
}


/**
 * @brief   运行由 compiler 编译得到的可执行文件。
 *          用户程序的运行时错误输出到 {filename}.stderr，
 *          用户程序的答案输出到 {filename}.stdout，
 *          用户程序的测试样例从 {filename}.stdin 输入。
 * @param file_name 不带后缀的文件名
 * @param time_limit 程序时间限制（单位：秒）
 * @param mem_limit 程序内存限制 （单位：KB） 
 * @return  返回用户程序运行情况：
 *          -1  fork 错误；
 *          -2  exec 错误；
 *          -3  重定向文件打开错误；
 *          -4  资源限制出错
 *          0   运行正常；
 *          > 0 用户程序退出时收到的信号；
 */
int run(const std::string& file_name,int time_limit,int mem_limit)
{  
    std::string _elf_path = path::get_elf(file_name);
    std::string _stdin_path = path::get_stdin(file_name);
    std::string _stdout_path = path::get_stdout(file_name);
    std::string _err_path = path::get_err(file_name);

    // 打开运行程序的输出输入文件。
    umask(0);
    int _out_fd = open(_stdout_path.c_str(),O_WRONLY | O_CREAT | O_TRUNC, 0664);
    int _in_fd = open(_stdin_path.c_str(),O_RDONLY,0664);
    int _err_fd = open(_err_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,0664);

    if(_out_fd < 0 or _in_fd < 0 or _err_fd < 0)
    {
        log(ERROR) << "Open file error" << std::endl;
        return -3; // open file error
    }

    // 子进程执行程序        
    pid_t _ch_id = fork();
    if(_ch_id < 0)
    {
        log(ERROR) << "Runner fork error" << std::endl;
        return -1; // fork error
    }

    if(_ch_id == 0) // 子进程
    {
        // 用户程序的输入输出都保存到文件中
        dup2(_in_fd,0);
        dup2(_out_fd,1);
        dup2(_err_fd,2);

        if(!set_limit(time_limit,mem_limit))
        {
            log(WARNING) << "Set resources error" << std::endl;
            return -4; // set resources error
        }
        // 执行用户代码
        execl(_elf_path.c_str(),file_name.c_str(),nullptr);

        // 替换失败
        log(ERROR) << "Runner exec failed." << std::endl;
        return -2; // exec error
    }

    int _ch_ret;
    // 父进程
    waitpid(_ch_id,&_ch_id,0); // 阻塞等待

    log(INFO) << "Run successfully, child get signal: " << (_ch_id & 0x7f) << std::endl;
    // 返回子进程终止信号
    return (_ch_id & 0x7f); // successfully run
}   

void run_server(const std::string& file_name,int time_limit,int mem_limit)
{
    compile(file_name);
    run(file_name,time_limit,mem_limit);
}

NAMESPACE_OJ_END