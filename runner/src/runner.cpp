#include "runner.hpp"
#include <string>   
int main()
{
    std::string usr_in = R"({
    "code":"
    #include<iostream>
    using namespace std;
    int main()
    {
        cout << \"hello world\" << endl;
        return 0;
    }",
    "time_limit":10,
    "mem_limit":1024
    })";

    std::string out;
    OJ::run_server(usr_in,out);

    std::cout << out << std::endl;
    return 0;
}