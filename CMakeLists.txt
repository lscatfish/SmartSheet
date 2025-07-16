cmake_minimum_required(VERSION 3.20)
project(SmartSheet)

# 放在 project() 之后即可
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
link_directories("D:/code/xlnt/build/source/Release")

add_executable(SmartSheet
"SmartSheet.cpp"
"SmartSheet.h"
"src/main.cpp"
"src/ChineseEncoding.cpp"
"include/ChineseEncoding.h"
"include/PersonnelInformation.h")
target_link_libraries(SmartSheet PRIVATE xlnt)