set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(QtFluentWidgets
        VERSION 0.1.1
        LANGUAGES CXX C
)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED
)

file(GLOB_RECURSE COMMON_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/common/*
)
file(GLOB_RECURSE COMPONENTS_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/components/*
)
list(APPEND SOURCE_FILES
        ${COMMON_FILES}
        ${COMPONENTS_FILES}
)
qt_add_big_resources(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/resource.qrc)

option(${PROJECT_NAME}_BUILD_STATIC "编译静态库而非动态库" OFF)

if (NOT DEFINED ${PROJECT_NAME}_BUILD_STATIC
        OR NOT ${${PROJECT_NAME}_BUILD_STATIC})
    # Build Shared Library
    add_library(QtFluentWidgets SHARED
            ${SOURCE_FILES}
    )
    message(STATUS "${PROJECT_NAME}_BUILD_STATIC is OFF, will build shared library.")
else ()
    # Build Static Library
    add_library(QtFluentWidgets STATIC
            ${SOURCE_FILES}
    )
    message(STATUS "${PROJECT_NAME}_BUILD_STATIC is ON, will build static library.")
endif ()

set_target_properties(QtFluentWidgets PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
)

target_link_libraries(QtFluentWidgets
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

## For Debug
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
