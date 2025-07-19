cmake_minimum_required(VERSION 3.20)
project(SmartSheet)

# 使用ninja编译时强制定义以下宏
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
    add_definitions(-D_AMD64_)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
    add_definitions(-D_ARM64_)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM")
    add_definitions(-D_ARM_)
else()
    add_definitions(-D_X86_)
endif()

# Windows SDK 可保留
set(CMAKE_SYSTEM_VERSION "10.0.22621.0" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 17)

include_directories("D:/code/xlnt/include")
include_directories("./include")
link_directories("D:/code/xlnt/build/source/Debug")
link_directories("D:/code/xlnt/build/source/Release")

add_executable(SmartSheet
"src/main.cpp"
"src/ChineseEncoding.cpp"
"include/ChineseEncoding.h"
"include/PersonnelInformation.h"
"include/Files.h"
"src/Files.cpp"
"include/QingziClass.h"
"src/QingziClass.cpp"
"src/test.cpp"
"include/test.h")

# 根据构建类型选择正确的库名
target_link_libraries(SmartSheet PRIVATE
    $<$<CONFIG:Debug>:xlntd>
    $<$<CONFIG:Release>:xlnt>
)
