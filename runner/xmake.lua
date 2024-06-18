add_rules("mode.debug", "mode.release")
add_includedirs("./include")
add_includedirs("$(projectdir)/../include")

target("run")
    set_kind("binary")
    add_files("src/*.cpp")
    set_rundir("$(projectdir)")




