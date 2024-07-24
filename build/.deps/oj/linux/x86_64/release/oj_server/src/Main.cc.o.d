{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-Iinclude",
            "-Ioj_server/include",
            "-isystem",
            "/home/ubuntu/.xmake/packages/j/jsoncpp/1.9.5/6a59121558c24c01a73b20670010be3a/include",
            "-DNDEBUG"
        }
    },
    files = {
        "oj_server/src/Main.cc"
    },
    depfiles_gcc = "Main.o: oj_server/src/Main.cc oj_server/include/Reactor.hpp  oj_server/include/socket.hpp include/log.hpp include/conf.hpp  oj_server/include/Connection.hpp oj_server/include/threadpool.hpp  oj_server/include/epoller.hpp oj_server/include/Handler.hpp  oj_server/include/http.hpp\
"
}