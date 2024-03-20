
-- Widgets Library
target("qt_line_widgets_static")
    add_rules("qt.static")
    add_files("common/*.{h, cc}", "components/*.{h, cc}")
    add_frameworks("QtGui")

-- todo