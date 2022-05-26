add_requires("cmake::ROOT", { alias = "ROOT", system = true })
target("test")
    set_kind("binary")
    set_languages("cxx14")
    add_files("src/main_hough.C")
    add_includedirs("include")
    set_pcxxheader("include/json.hpp")
    set_pcxxheader("include/clipp.h")
    add_packages("ROOT")


option()
    add_defines("DEBUG")

