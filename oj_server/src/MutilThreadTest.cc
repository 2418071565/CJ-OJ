#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <vector>

#include "Reactor.hpp"
#include "socket.hpp"

TEST(T1,t1) {
    // 创建线程模拟用户访问
    std::vector<std::thread> Usrs(10);
    for(auto&usr:Usrs) {
        usr = [] {
            CJ::sock sk()
            for(int i = 0;i < 10000;++i) {
                
            }
        }
    }
    
    // 创建服务端
    CJ::Reactor Ser;
    Ser.init();



}
