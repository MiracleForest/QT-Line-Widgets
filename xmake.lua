-- Set Project
set_project("Qt-Line-Weight")
set_version("0.0.1", {build = "%Y%m%d-%H%M"})

-- Set Common Options
set_warnings("all", "error")

-- Set Build Modes
add_rules("mode.release", "mode.debug")

-- Include SubTargets
includes("widgets", "examples")