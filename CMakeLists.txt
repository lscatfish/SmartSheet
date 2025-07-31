cmake_minimum_required(VERSION 3.20)

# 如果支持，请为 MSVC 编译器启用热重载。
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:DEBUG,RELWITHDEBINFO>:EditAndContinue>")
    endif()
endif()

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

include_directories("./include")

# 如果可以，请使用绝对路径
# 如果可以，请更改的xlnt调用的路径
include_directories("./third_party/xlnt/include")
link_directories("./third_party/xlnt/lib/Debug")
link_directories("./third_party/xlnt/lib/Release")

add_executable(SmartSheet
"main.cpp"
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
    $<$<CONFIG:Release>:xlnt>)

# 根据配置选择复制哪个 DLL
add_custom_command(TARGET SmartSheet POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/third_party/xlnt/lib/$<CONFIG>/xlnt$<$<CONFIG:Debug>:d>.dll"
            $<TARGET_FILE_DIR:SmartSheet>)