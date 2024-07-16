#pragma once
#include "Connection.hpp"
#include <jsoncpp/json/json.h>

CJOJ_BEGIN

class httpServer;

// http 请求
class httpRequest
{
public:
    // 解析 http 请求
    int decode(const std::shared_ptr<Connection>& __con)
    {
        std::stringstream& ibf = __con->inBuffer(); // 输入缓冲区
        Json::Value& req = __con->request();    // 上次解析到一般的 htpp

        // 头部已经读完，就不需要一行一行读了，只需要读够字节数即可
        if(__con->isHandOver())
        {
readContent:
            std::string cont = req["Content"].asString();       // 已经读到的数据
            uint32_t contLen = req["Content-Length"].asUInt();  // 内容这字节数

            // 读完缓冲区或者字节数读够了，就结束
            while((!ibf.eof()) and cont.size() < contLen)
            {
                char buff[1024] = { 0 };
                ibf.read(buff,min(sizeof(buff) - 1,contLen - cont.size()));
                cont += buff;
            }
            req["Content"] = cont;
            if(cont.size() == contLen)
            {
                // 完整读完一个有请求体的请求，解析完成
                return 0;
            }
            return -1; 
        }

        // 读请求头
        while(ibf.eof())
        {
            std::string line;
            std::getline(ibf,line);
            if(ibf.eof())
            {
                ibf << line;
                return -1;    // 没有完整的一行
            }
            
            // 读到的第一行是请求行
            if(req.empty())
            {
                std::stringstream ss(line);
                // 请求方法，http 版本，url
                std::string mth,ver,url;    
                ss >> mth >> url >> ver;
                req["httpMethod"] = mth;
                req["httpURL"] = url;
                req["httpVersion"] = ver;
                continue;
            }

            // 已经读完请求头
            if (line == "\r")
            {
                // 如果有数据，就读取数据
                if(req.isMember("Content-Length"))
                {
                    __con->setHandlOver(true);
                    req["Content"] = "";
                    goto readContent;
                }
                else 
                {
                    // 完整读完一个没有请求体的请求，解析完成
                    return 0;
                }
            }

            // 其他请求头
            int pos = line.find(':');
            std::string handName = line.substr(0,pos);
            std::string handValue = line.substr(pos + 1,line.size() - pos - 2);
            req[handName] = handValue;
        }
    }
};


// http 响应，用于构造响应报文
class httpRespone
{
    Json::Value respone;
public:
    httpRespone()
        :
};

// 提供 htpp 服务，处理 http 请求并构造相应的 http 响应
class httpServer
{
public:
    static void Router(const Json::Value& usrData)
    {
        std::string url = usrData["httpURL"].asString();
        std::string method = usrData["httpMethod"].asString();
        std::string version = usrData["httpVersion"].asString();

        if(method == "GET")
        {

        }
    }

    void Serve(const std::shared_ptr<Connection>& __con)
    {

    }

};

CJOJ_END
