add_includedirs("./include")

target("oj")
    set_kind("binary")
    add_files("src/*.cc")
    add_packages("jsoncpp")
    set_rundir("$(projectdir)/oj_server")