cmake_minimum_required(VERSION 3.21)

project(QtFluentWidgets
        VERSION 0.1.1
        LANGUAGES CXX C
)

## 设置QT安装目录 >= QT6
if ("" STREQUAL "${QT_CMAKE_DIR}")
    set(CMAKE_PREFIX_PATH  "D:/Bin/QT/6.2.4/mingw_64")
else ()
    set(CMAKE_PREFIX_PATH  ${QT_CMAKE_DIR})
endif ()
message(STATUS "QT cmake directory: ${CMAKE_PREFIX_PATH}")

## 第三方库目录
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)

## 组件目录
add_subdirectory(widgets)

## 允许测试
enable_testing()

## 示例目录
add_subdirectory(examples)