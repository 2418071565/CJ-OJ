#pragma once

#include <string>
#include <filesystem>
#include "log.hpp"
#include "init.hpp"


NAMESPACE_OJ_BEGIN

namespace util
{ // namespace util begin


namespace path
{   // namespace path begin

    const std::string _prefix = "./tmp/"; // 路径前缀

    // 获取源文件路径
    inline std::string get_src(const std::string& _file)
    { return _prefix + _file + ".cpp";}

    // 获取可执行文件路径
    inline std::string get_elf(const std::string& _file)
    { return _prefix + _file; }

    // 获取编译错误输出文件路径
    inline std::string get_comp_err(const std::string& _file)
    { return _prefix + _file + ".comp_err";}

    // 获取编译错误文件路径
    inline std::string get_err(const std::string& _file)
    { return _prefix + _file + ".err"; }

    // 获取运行程序的标准输入路径
    inline std::string get_stdin(const std::string& _file)
    { return _prefix + _file + ".stdin";}

    // 获取运行程序的标准输输出路径
    inline std::string get_stdout(const std::string& _file)
    { return _prefix + _file + ".stdout";}



}   // namespace path end


}   // namespace util end


NAMESPACE_OJ_END
