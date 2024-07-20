#include "OJServer.hpp"
#include "Reactor.hpp"

int main()
{
    CJ::Reactor ss;
    ss.init();
    ss.start();
    return 0;
}
