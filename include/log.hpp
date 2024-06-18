#pragma once
#include <string>
#include <iostream>
#include <ctime>
#include <mutex>
#include "init.hpp"

NAMESPACE_OJ_BEGIN

enum
{
    INFO,
    DEGUB,
    WARNING,
    ERROR,
    FATAL,
};

/**
 * @brief 一个简单的日志系统。线程安全。 
 */
class __LOG
{
public:
    std::mutex _out_mt;

    std::ostream& operator()(const std::string& _level,
                        const std::string& _file,int _line) 
    {
        std::string _msg;

        // 获取时间戳，并格式化
        std::time_t __t = time(nullptr);
        tm* _tm = gmtime(&__t);
        // struct tm
        // {
        //     int tm_sec;  /*秒，正常范围0-59， 但允许至61*/
        //     int tm_min;  /*分钟，0-59*/
        //     int tm_hour; /*小时， 0-23*/
        //     int tm_mday; /*日，即一个月中的第几天，1-31*/
        //     int tm_mon;  /*月， 从一月算起，0-11*/  1+p->tm_mon;
        //     int tm_year;  /*年， 从1900至今已经多少年*/  1900＋ p->tm_year;
        //     int tm_wday; /*星期，一周中的第几天， 从星期日算起，0-6*/
        //     int tm_yday; /*从今年1月1日到目前的天数，范围0-365*/
        //     int tm_isdst; /*日光节约时间的旗标*/
        // };
        char _tm_buff[128];
        snprintf(_tm_buff,sizeof(_tm_buff),"%d-%d-%d-%d:%d:%d",
                _tm->tm_year + 1900/*年*/,_tm->tm_mon + 1/*月*/,
                    _tm->tm_mday/*日*/,_tm->tm_hour/*小时*/,
                        _tm->tm_min/*分钟*/,_tm->tm_sec/*秒*/);

        _msg = "[" + _level + "]" + "[ " + _file + "\t]" + 
                "[" + std::to_string(_line) + "]" + "[" + _tm_buff + "]: ";
        {
            std::unique_lock<std::mutex> lg(_out_mt);
            std::cout << _msg;
        }
        return std::cout;
    }
};

__LOG LOG;

#define log(level) LOG(#level, __FILE__ , __LINE__)     // 输出日志消息



NAMESPACE_OJ_END