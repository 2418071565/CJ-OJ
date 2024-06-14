add_rules("mode.debug", "mode.release") 
add_includedirs("../comm")


target("run")
    set_kind("binary")
    add_files("*.cpp")


