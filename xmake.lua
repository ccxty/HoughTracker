add_requires("cmake::ROOT", { alias = "ROOT", system = true })
add_rules("mode.debug", "mode.release")

set_languages("cxx14")
set_targetdir("bin")

add_packages("ROOT")
add_includedirs("include")

option()
    add_defines("DEBUG")

    
target("clipp")
    set_kind("shared")
    set_targetdir("lib64")
    add_files("src/clipp.cpp")

target("json")
    set_kind("shared")
    set_targetdir("lib64")
    add_files("src/json.cpp")

target("args")
    set_kind("shared")
    set_targetdir("lib64")
    add_files("src/args.cpp")
    add_deps("clipp", "json")

target("lib")
    set_kind("shared")
    set_targetdir("lib64")
    add_files("src/*.cpp")

target("HoughTracker")
    set_kind("binary")
    set_targetdir("bin")
    add_files("main/main_hough.cpp")
    add_deps("lib")

target("LocalTracker")
    set_kind("binary")
    set_targetdir("bin")
    add_files("main/main_local.cpp")
    add_deps("lib")
