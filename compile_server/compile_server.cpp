#include "compiler.hpp"
#include "runner.hpp"

int main()
{
    OJ::compiler::compile("code");
    OJ::runner::run("code",1,1024);
    return 0;
}