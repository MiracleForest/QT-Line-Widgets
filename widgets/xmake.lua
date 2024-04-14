
-- Widgets Library
target("qt_line_widgets_static")
    set_kind("static")
    set_languages("c++20")
    add_cxxflags("/source-charset:utf-8", { tools = {"cl", "win32_msvc"}}, {force = true})
    add_rules("qt.static")
    add_includedirs(".", { public = true })
    add_files("res/resource.qrc")
    add_files("./common/**.h", "./common/**.cc")
    add_files("./components/**.h", "./components/**.cc")
    add_frameworks("QtGui", "QtCore", "QtWidgets", "QtSvg")
-- todo