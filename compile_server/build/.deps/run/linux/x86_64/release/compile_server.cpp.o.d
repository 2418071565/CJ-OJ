{
    depfiles_gcc = "compile_server.o: compile_server.cpp compiler.hpp ../comm/init.hpp  ../comm/util.hpp ../comm/log.hpp runner.hpp\
",
    files = {
        "compile_server.cpp"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-DNDEBUG"
        }
    }
}