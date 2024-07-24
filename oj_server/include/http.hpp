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
    // 返回 0 正确解析，-1 数据不完整
    static int decode(const std::shared_ptr<Connection>& __con)
    {
        std::stringstream& ibf = __con->inBuffer(); // 输入缓冲区
        Json::Value& req = __con->request();    // 上次解析到一半的 http 报文

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
                ibf.read(buff,std::min(sizeof(buff) - 1,contLen - cont.size()));
                cont += buff;
            }
            req["Content"] = cont;
            if(cont.size() == contLen)
            {
                // 完整读完一个有请求体的请求，解析完成
                __con->setHandOver(false);  // 恢复状态
                return 0;
            }
            return -1; 
        }

        // 读请求头
        while(!ibf.eof())
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

            // 读到空行，已经读完请求头
            if (line == "\r")
            {
                // 如果有数据，就读取数据
                if(req.isMember("Content-Length"))
                {
                    __con->setHandOver(true);
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
            // 错误数据，根本不是 http 请求
            if(pos == -1)
            {
                req.clear(); // 清空之前的请求
                return -1;
            }
            std::string handName = line.substr(0,pos);
            std::string handValue = line.substr(pos + 1,line.size() - pos - 2);
            req[handName] = handValue;
        }
        return -1;
    }
};

std::string webSite = R"(
<!DOCTYPE html>
<html lang="ch">
<head>
    <meta charset="UTF-8">
    <title>cyb</title>
</head>
<body>
    <h1><strong>你好!</strong></h1>
</body>
</html>
)";


// http 响应，用于构造响应报文
class httpRespone
{
    std::string respone;
    Json::Value responeJson;
public:
    // 生成响应头
    httpRespone(const std::string& Version = "HTTP/1.1",int Status = 200,const std::string& Description = "OK")
    { respone += Version + ' ' + std::to_string(Status) + ' ' + Description + "\r\n"; }


    // 在设置完内容后就不允许设置其他内容了
    void setContent(const std::string& ContentType,uint32_t ContentLength,const std::string& Content) noexcept
    {
        responeJson["Content-Type"] = ContentType;
        responeJson["Content-Length"] = std::to_string(ContentLength);
        responeJson["Content"] = Content;
    }

    void setKeepAlive() noexcept
    {
        responeJson["Keep-Alive"] = "timeout=5,max=1000";
    }

    // 返回 http 报文字符串
    std::string encode() noexcept
    {
        for(auto&x:responeJson.getMemberNames())
        {
            if(x == "Content")continue;
            respone += x + ":" + responeJson[x].asString() + "\r\n";
        }
        respone += "\r\n";
        if(responeJson.isMember("Content"))respone += responeJson["Content"].asString();
        return respone;
    }
};

// 提供 htpp 服务，处理 http 请求并构造相应的 http 响应
class httpServer
{
public:
     static void Router(const Json::Value& usrData) { }  // 暂时没用，以后做扩展

    static void Serve(const std::shared_ptr<Connection>& __con)
    {
        if(httpRequest::decode(__con) < 0)return ;  // 返回 -1 解析失败，不能做处理

        Json::Value& usrData = __con->request();
        std::string url = usrData["httpURL"].asString();
        std::string method = usrData["httpMethod"].asString();
        std::string version = usrData["httpVersion"].asString();

        if(method == "GET")
        {
            if(url == "/")
            {
                httpRespone rsp;
                rsp.setContent("text/html",webSite.size(),webSite);
                __con->append_outbuff(rsp.encode());
            }
            else 
            {
                httpRespone rsp("HTTP/1.1",404,"Not Found");
                __con->append_outbuff(rsp.encode());
            }
        }
    }

};

CJOJ_END