add_rules("mode.debug", "mode.release") 
add_requires("jsoncpp 1.9.5")
add_includedirs("./include")
set_xmakever("2.6.9")
includes("oj_server/xmake.lua")    -- 添加子项目

set_languages("cxx17")



