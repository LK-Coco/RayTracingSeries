add_rules("mode.debug", "mode.release")
set_languages("c++20")

target("cray")
    set_kind("binary")
    add_includedirs("src")
    add_files("src/**.cpp")
    set_rundir("./")
