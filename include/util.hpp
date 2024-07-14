#pragma once

#include <string>
#include <filesystem>
#include <random>
#include <ctime>
#include <fstream>
#include <string>
#include <iostream>
#include <chrono>
#include <signal.h>
#include <atomic>
#include "log.hpp"
#include "conf.hpp"


CJOJ_BEGIN

namespace util
{ // namespace util begin


namespace path
{   // namespace path begin

    const std::string _prefix = "./tmp/"; // 路径前缀

    // 获取源文件路径
    inline std::string src(const std::string& _file)
    { return _prefix + _file + ".cpp";}

    // 获取可执行文件路径
    inline std::string elf(const std::string& _file)
    { return _prefix + _file; }

    // 获取编译错误输出文件路径
    inline std::string complie(const std::string& _file)
    { return _prefix + _file + ".complie";}

    // 获取标准错误文件路径
    inline std::string err(const std::string& _file)
    { return _prefix + _file + ".err"; }

    // 获取运行程序的标准输入路径
    inline std::string in(const std::string& _file)
    { return _prefix + _file + ".stdin";}

    // 获取运行程序的标准输输出路径
    inline std::string out(const std::string& _file)
    { return _prefix + _file + ".stdout";}


}   // namespace path end


namespace file
{ // namespace file begin

    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch()
    );

    // 获取唯一文件名
    std::atomic_uint32_t _cnt(0);
    inline std::string unique_file()
    {
        return std::to_string(ms.count()) + std::to_string(++_cnt);
    }

    // 写文件
    inline bool write_file(const std::string& path,const std::string& in)
    {
        std::ofstream ofs(path,std::ios::out);
        if(!ofs.is_open())
            return false;
        ofs.write(in.c_str(),in.size());
        ofs.close();
        return true;
    }

    // 读文件全部内容
    inline bool read_file(const std::string& path,std::string& out)
    {
        std::ifstream ifs(path,std::ios::in);
        if(!ifs.is_open())
            return false;
        out.clear();
        out = std::string(std::istreambuf_iterator<char>(ifs),
					    std::istreambuf_iterator<char>());
        ifs.close();
        return true;
    }

} // namespace file end

/**
 * @param code  code > 0 用户程序异常收到的信号，code = 0 正常运行，code < 0 服务器运行异常
 * 
 */
inline std::string code_to_msg(int code,const std::string& file_name)
{
    switch (code)
    {
        case SIGXCPU:   return "运行超时"; 
        case SIGSEGV:   return  "段错误";
        case SIGABRT:   return "内存超限"; 
        case 0:         return "运行成功";
        case -1:        
        case -2:        
        case -3:        
        case -4:        return "未知错误";
        case -5:        return "JSON 格式错误";
        case -6:        return "代码为空";
        case -7:        
        {
            std::string res;
            if(!file::read_file(path::complie(file_name),res))
            {
                log(ERROR) << "Complie error file open failed." << std::endl;
                return "编译错误";
            }
            return res;
        }
        default:        return "未知状态码：" + std::to_string(code);
    }
}

}   // namespace util end


CJOJ_END
