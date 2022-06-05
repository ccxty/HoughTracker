add_requires("cmake::ROOT", { alias = "ROOT", system = true })
add_rules("mode.debug", "mode.release")

set_languages("cxx14")
set_targetdir("bin")
add_packages("ROOT")
add_includedirs("include")
set_pcxxheader("include/pch.h")

target("hough")
    set_kind("binary")
    add_files("src/main_hough.C")

target("local")
    set_kind("binary")
    add_files("src/main_local.C")

option()
    add_defines("DEBUG")

