target("example")
	add_rules("qt.widgetapp")
	add_deps("qt_line_widgets_static")
	add_files("test.cc")