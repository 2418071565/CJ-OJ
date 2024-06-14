#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>
#include <fcntl.h>
#include "init.hpp"
#include "util.hpp"
#include "log.hpp"


NAMESPACE_OJ_BEGIN

namespace compiler
{ // oj_compiler begin

using namespace util;


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

} // oj_compiler end



NAMESPACE_OJ_END