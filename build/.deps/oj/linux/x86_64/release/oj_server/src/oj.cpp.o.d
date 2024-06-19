{
    files = {
        "oj_server/src/oj.cpp"
    },
    depfiles_gcc = "oj.o: oj_server/src/oj.cpp oj_server/include/server.hpp include/init.hpp  include/log.hpp include/util.hpp oj_server/include/runner.hpp\
",
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
            "-isystem",
            "/home/ubuntu/.xmake/packages/c/cpp-httplib/0.15.3/10d0838bab72482e8b727994616a9af6/include",
            "-DNDEBUG"
        }
    }
}