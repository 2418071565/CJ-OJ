add_includedirs("./include")

target("oj")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("jsoncpp","cpp-httplib")
    set_rundir("$(projectdir)/oj_server")