{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-g",
            "-O0",
            "-Iinclude",
            "-I/home/cyb/online_judge/runner/../include",
            "-isystem",
            "/home/cyb/.xmake/packages/j/jsoncpp/1.9.5/6a59121558c24c01a73b20670010be3a/include"
        }
    },
    depfiles_gcc = "runner.o: src/runner.cpp include/runner.hpp  /home/cyb/online_judge/runner/../include/init.hpp  /home/cyb/online_judge/runner/../include/util.hpp  /home/cyb/online_judge/runner/../include/log.hpp\
",
    files = {
        "src/runner.cpp"
    }
}