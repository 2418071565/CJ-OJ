#include "Reactor.hpp"

int main()
{
    CJ::Reactor ss;
    ss.init();
    try {
        ss.start();
    } catch (std::exception e) {
        std::cout << "+++++++++++++++++++++++++\n" << std::endl;
        std::cout << e.what() << std::endl;
    }
    return 0;
}
