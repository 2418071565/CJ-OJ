{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-Iinclude",
            "-Irunner/include",
            "-isystem",
            "/home/cyb/.xmake/packages/j/jsoncpp/1.9.5/6a59121558c24c01a73b20670010be3a/include",
            "-DNDEBUG"
        }
    },
    files = {
        "runner/src/runner.cpp"
    },
    depfiles_gcc = "runner.o: runner/src/runner.cpp runner/include/runner.hpp  include/init.hpp include/util.hpp include/log.hpp\
"
}