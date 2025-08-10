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


# 找包
find_package(OpenCV REQUIRED)
# 这里xlnt包不知道什么原因win没法安装，只能分步了
#set(Xlnt_DIR "")
#find_package(Xlnt REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/third_party/xlnt/include/")

# 找到 src 文件夹下所有的 .cpp 文件
file(GLOB SOURCES "src/*.cpp" "include/*.h" "include/*.hpp")

# 添加执行文件
add_executable(${PROJECT_NAME} "main.cpp" ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/third_party/xlnt/Debug/xlntd.lib>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/third_party/xlnt/Release/xlnt.lib>
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

# message(STATUS "Xlnt_INCLUDE_DIRS = ${Xlnt_INCLUDE_DIRS}")
# message(STATUS "xlnt include dirs: $<TARGET_PROPERTY:xlnt::xlnt,INTERFACE_INCLUDE_DIRECTORIES>")

# 根据当前配置选择 DLL
set(XLNT_DLL_PATH
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/third_party/xlnt/Debug/xlntd.dll>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/third_party/xlnt/Release/xlnt.dll>
)

# 把 DLL 拷贝到目标可执行文件目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${XLNT_DLL_PATH}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying xlnt dll to output directory"
)