add_rules("mode.debug", "mode.release")
add_requires("jsoncpp 1.9.5","cpp-httplib 0.15.3")
add_includedirs("./include")

target("run")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("jsoncpp")
    set_rundir("$(projectdir)/runner")

