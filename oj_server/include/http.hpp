#pragma once
#include "Connection.hpp"
#include <jsoncpp/json/json.h>

CJOJ_BEGIN

class http_req
{
public:
    static void decode(const std::shared_ptr<Connection>& __con)
    {
        Json::Value _M_js;
        // __con->
    }

};



class http_rsp
{
    Json::Value _M_js;
public:
    
};

CJOJ_END
