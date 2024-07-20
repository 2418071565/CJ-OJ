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
    int decode(const std::shared_ptr<Connection>& __con)
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
                ibf.read(buff,min(sizeof(buff) - 1,contLen - cont.size()));
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

std::string webSite = R"(
<!DOCTYPE html>
<html lang="ch">
<head>
    <meta charset="UTF-8">
    <title>CYB's BLOG</title>
    <link rel="stylesheet" href="./css/font.css">
    <link rel="stylesheet" href="./css/index.css" />
    <link rel="stylesheet" href="./css/iconfont.css"/>

    <script>
        document.addEventListener('DOMContentLoaded', function() {
            const element = document.querySelector('.image-text');
            element.classList.add('visible');
        }); 
    </script>
</head>
<body>
    <header class="b-header">
        <a href="./index.html" class="b-header-logo"></a>
        <span class="b-header-name" style="user-select: none;">Made By CYB</span>
        <div class="container">
            <nav class="b-header-nav">
                <a href="https://2418071565.github.io/DS_Algo/" class="nav-item">
                    数据结构<span class="iconfont icon-xiala"></span>
                    <div class = "layer">
                        堆<br>
                        并查集<br>
                        平衡二叉树<br>
                        哈希表<br>
                        线段树<br>
                    </div>
                </a>
                <a href="https://2418071565.github.io/C%2B%2B/intro/" class="nav-item">C++</a>
                <a href="https://2418071565.github.io/OSindex/" class = "nav-item">LINUX</a>
                <a href="https://2418071565.github.io/Others/Git/" class="nav-item">其他</a>
            </nav>
            <div class="b-header-search">
                <form action="">
                    <input type="text" placeholder="搜索" />
                    <a href="" class="btn">
                        <span class="iconfont icon-search"></span>
                    </a>
                </form>
            </div>
        </div>
        <div class="b-header-icons">
            <a href="https://2418071565.github.io/" class="icons-item">
                <img src="./assets/github.png" alt="">
                <!--                    <span class="tit">GITHUB</span>-->
            </a>
            <a href="https://2418071565.github.io/" class="icons-item">
                <img src="./assets/avatar.png" alt="">
            </a>
        </div>
    </header>

    <main class="b-main" style="background: linear-gradient(to left, rgb(142, 158, 171), rgb(238, 242, 243));">
        <div class="image-text">
            <h1>Welcome to CYB's Blog</h1>
            <p us>Good Good Study,Day Day Up</p>
        </div>

    </main>
            
    <footer>
        <p>&copy;Provided by Chen Yibo</p>
    </footer>

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
    httpRespone(const std::string& Version = "1.1",int Status = 200,const std::string& Description = "OK")
    { respone += Version + ' ' + std::to_string(Status) + ' ' + Description + '\r\n'; }


    // 在设置完内容后就不允许设置其他内容了
    void setContent(const std::string& ContentType,uint32_t ContentLength,const std::string& Content) noexcept
    {
        responeJson["Content-Type"] = ContentType;
        responeJson["Content-Length"] = std::to_string(ContentLength);
        responeJson["Content"] = Content;
    }

    // 返回 http 报文字符串
    std::string encode() noexcept
    {
        respone += "Content-Type:" + responeJson["Content-Type"].asString() + '\r\n';
        respone += "Content-Length:" + responeJson["Content-Length"].asString() + '\r\n';
        respone += '\r\n';
        respone += responeJson["Content"].asString();
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
        }
    }

};

CJOJ_END
