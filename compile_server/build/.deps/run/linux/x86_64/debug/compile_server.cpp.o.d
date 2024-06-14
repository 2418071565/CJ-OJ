{
    files = {
        "compile_server.cpp"
    },
    depfiles_gcc = "compile_server.o: compile_server.cpp compiler.hpp ../comm/init.hpp  ../comm/util.hpp ../comm/log.hpp runner.hpp\
",
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-g",
            "-O0",
            "-I../comm"
        }
    }
}