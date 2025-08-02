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

set(Xlnt_DIR "${CMAKE_SOURCE_DIR}/third_party/xlnt/x64-Release/lib/cmake/xlnt")

# 找包
find_package(OpenCV REQUIRED)
find_package(Xlnt REQUIRED)

include_directories("./include")


# 添加执行文件
add_executable(${PROJECT_NAME}
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

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE 
xlnt::xlnt
${OpenCV_LIBS})

# message(STATUS "Xlnt_INCLUDE_DIRS = ${Xlnt_INCLUDE_DIRS}")
# message(STATUS "xlnt include dirs: $<TARGET_PROPERTY:xlnt::xlnt,INTERFACE_INCLUDE_DIRECTORIES>")

# 复制 DLL
add_custom_command(TARGET SmartSheet POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:xlnt::xlnt>
            $<TARGET_FILE_DIR:SmartSheet>)