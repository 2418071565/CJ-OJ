add_includedirs("./include")


target("oj")
    set_kind("binary")
    add_files("src/Main.cc")
    add_packages("jsoncpp gtest")
    set_rundir("$(projectdir)/oj_server")
    set_runargs("8888")


target("test")
    set_kind("binary")
    add_files("src/MutilThreadTest.cc")
    add_packages("gtest")
    add_links("gtest_main")
    set_rundir("$(projectdir)/oj_server")