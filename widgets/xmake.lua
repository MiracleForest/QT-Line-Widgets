
-- Widgets Library
target("qt_line_widgets_static")
    set_kind("static")
    set_languages("c++20")
    add_rules("qt.static")
    add_frameworks("Qt6Gui", "Qt6Core")
    add_includedirs(".", { public = true })
    add_files("./common/**.h", "./common/**.cc")
    add_files("./components/**.h", "./components/**.cc")

-- todo