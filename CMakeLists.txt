
cmake_minimum_required(VERSION 3.20)

# 打印原始系统名称（用于调试）
message(STATUS "调试信息: CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "调试信息: CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")

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
    add_definitions(-D_AMD64_)          # x64位
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
    add_definitions(-D_ARM64_)          # arm64位
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM")
    add_definitions(-D_ARM_)
else()
    add_definitions(-D_X86_)            # x86位
endif()

# Windows SDK 可保留
set(CMAKE_SYSTEM_VERSION "10.0.22621.0" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 优化选项设置代码
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  # 区分编译器类型设置优化选项
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC 或 Clang 使用 -O3
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
  elseif(MSVC)
    # MSVC 使用 /O2（对应 GCC 的 -O3 级别优化）
    # /O2：最快速度优化（MSVC 的默认 Release 优化级别）
    # /Ox：最大化优化（比 /O2 更激进）
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /arch:AVX2")
  endif()
  
  # 启用 LTO（链接时优化）
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported)
  if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()


# 找包，这是可行的
# find_package(OpenCV REQUIRED)

# 这里xlnt包不知道什么原因win没法安装，只能分步了
#set(Xlnt_DIR "")
#find_package(Xlnt REQUIRED)

# 由于尽量满足简单地可移植性这一条件，我都将第三方库安装的这里

set(THIRD_PARTY_DLL_DIR "${CMAKE_SOURCE_DIR}/third_party/dll/")
set(THIRD_PARTY_LIB_DIR "${CMAKE_SOURCE_DIR}/third_party/lib/")

# 根据构建类型选择 FTXUI 库路径
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(FTXUI_PATH "${THIRD_PARTY_LIB_DIR}/ftxui-6.1.9-win64/Debug")
else()
  set(FTXUI_PATH "${THIRD_PARTY_LIB_DIR}/ftxui-6.1.9-win64/Release")
endif()
include_directories(${FTXUI_PATH}/include)
link_directories(${FTXUI_PATH}/lib)

include_directories("${CMAKE_SOURCE_DIR}/include/")
include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/capi_wrapper/")
include_directories("${THIRD_PARTY_DLL_DIR}/xlnt/include/")
include_directories("${THIRD_PARTY_DLL_DIR}/opencv/include/")
include_directories("${THIRD_PARTY_DLL_DIR}/zlib/include/")
include_directories("${THIRD_PARTY_DLL_DIR}/icu/include/")
include_directories("${THIRD_PARTY_DLL_DIR}/poppler-25.07.0/Library/include")
include_directories("${THIRD_PARTY_LIB_DIR}/ftxui-6.1.9-win64/include")

# 找到 src 文件夹下所有的 .cpp 文件
file(GLOB SOURCES   "src/*.cpp" "include/*.h" "include/*.hpp" "src/*.c"
                    "test/*.h" "test/*.hpp" "test/*.c" "test/*.cpp"
)

file(GLOB PY_WRAPPER "src/*.cpp" "include/*.h" "include/*.hpp" "src/*.c"
                    "test/*.h" "test/*.hpp" "test/*.c" "test/*.cpp"
                    "capi_wrapper/*.h" "capi_wrapper/*.cpp" "capi_wrapper/*.c"
)

file(GLOB SEARCHINGTOOL_DIR "searchingTool/*.hpp" "searchingTool/*.h" 
"searchingTool/*.cpp" "searchingTool/*.c")

########################################Windows下可运行，linux下需要重新写##########################################
########################################Windows下可运行，linux下需要重新写##########################################
########################################Windows下可运行，linux下需要重新写##########################################
########################################Windows下可运行，linux下需要重新写##########################################
# windows下可运行，linux下需要重新写
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")


##################################OCR的相关文件############################################
# ppOCR的相关动态链接库
# 首先检查OCR DLL目录是否存在，然后查找文件
set(OCR_DLL_DIR "${THIRD_PARTY_DLL_DIR}/OCRdlls-Release")
if(NOT EXISTS ${OCR_DLL_DIR})
    message(WARNING "OCR DLL directory not found: ${OCR_DLL_DIR}")
else()
    # 使用LIST_DIRECTORIES false确保只获取文件
    file(GLOB OCR_DLL_RELEASE_FILES LIST_DIRECTORIES false "${OCR_DLL_DIR}/*.dll")
    
    # 验证是否找到了DLL文件
    if(NOT OCR_DLL_RELEASE_FILES)
        message(WARNING "No OCR DLL files found in ${OCR_DLL_DIR}")
    else()
        message(STATUS "Found ${CMAKE_ARGC} OCR DLL files to copy")
    endif()
endif()

#ppOCR的相关模型，（切勿更改models的文件夹名称）
set(SOURCE_OCR_MODELS "${CMAKE_SOURCE_DIR}/models")
##################################OCR的相关文件############################################

##################################icu的相关文件############################################
# icu的相关动态链接库
# 首先检查icu DLL目录是否存在，然后查找文件
set(ICU_DLL_DIR "${THIRD_PARTY_DLL_DIR}/icu/Release/bin64")
if(NOT EXISTS ${ICU_DLL_DIR})
    message(WARNING "ICU DLL directory not found: ${ICU_DLL_DIR}")
else()
    # 使用LIST_DIRECTORIES false确保只获取文件
    file(GLOB ICU_DLL_RELEASE_FILES LIST_DIRECTORIES false "${ICU_DLL_DIR}/*.dll")
    
    # 验证是否找到了DLL文件
    if(NOT ICU_DLL_RELEASE_FILES)
        message(WARNING "No ICU DLL files found in ${ICU_DLL_DIR}")
    else()
        message(STATUS "Found ${CMAKE_ARGC} ICU DLL files to copy")
    endif()
endif()
##################################icu的相关文件############################################

################################poppler的相关文件##########################################
# icu的相关动态链接库
# 首先检查icu DLL目录是否存在，然后查找文件
set(POPPLER_DLL_DIR "${THIRD_PARTY_DLL_DIR}/poppler-25.07.0/Library/bin")
if(NOT EXISTS ${POPPLER_DLL_DIR})
    message(WARNING "poppler DLL directory not found: ${POPPLER_DLL_DIR}")
else()
    # 使用LIST_DIRECTORIES false确保只获取文件
    file(GLOB POPPLER_DLL_RELEASE_FILES LIST_DIRECTORIES false 
    "${POPPLER_DLL_DIR}/*.dll"
    )
    
    # 验证是否找到了DLL文件
    if(NOT POPPLER_DLL_RELEASE_FILES)
        message(WARNING "No poppler DLL files found in ${POPPLER_DLL_DIR}")
    else()
        message(STATUS "Found ${CMAKE_ARGC} poppler DLL files to copy")
    endif()
endif()
################################poppler的相关文件##########################################


# 添加执行文件
add_executable(${PROJECT_NAME} "main.cpp" ${SOURCES})
add_executable(SearchingTool 
${SEARCHINGTOOL_DIR}
${SOURCES}
)

#add_library(SM SHARED ${PY_WRAPPER})

# 设置动态库输出目录（可选，统一输出到bin目录）
#set_target_properties(SM PROPERTIES
#    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin  # Linux/macOS
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin  # Windows DLL
#)

# 暴露头文件目录（供外部调用者使用）
#target_include_directories(SM
#    PUBLIC
#        ${CMAKE_CURRENT_SOURCE_DIR}/include  # 公开include目录
#    PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/src      # 私有源文件目录（可选）
#)

function(project_link_libraries TARGET_NAME)
# 链接lib
# 链接到xlnt
target_link_libraries(${TARGET_NAME} PRIVATE
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/xlnt/Debug/xlntd.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/xlnt/Release/xlnt.lib>
)
# 链接到opencv
target_link_libraries(${TARGET_NAME} PRIVATE 
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/opencv/lib/opencv_world4100d.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/opencv/lib/opencv_world4100.lib>
)
# 链接到zlib
target_link_libraries(${TARGET_NAME} PRIVATE 
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/zlib/Debug/zlibwapi.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/zlib/Release/zlibwapi.lib>
)
# 链接到icu
target_link_libraries(${TARGET_NAME} PRIVATE 
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/icu/Release/lib64/icudt.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/icu/Release/lib64/icuin.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/icu/Release/lib64/icuio.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/icu/Release/lib64/icutest.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/icu/Release/lib64/icutu.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/icu/Release/lib64/icuuc.lib>
)
# 链接到poppler
target_link_libraries(${TARGET_NAME} PRIVATE 
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/poppler-25.07.0/Library/lib/poppler.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/poppler-25.07.0/Library/lib/poppler-cpp.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/poppler-25.07.0/Library/lib/poppler-glib.lib>
)
# 链接到ftxui
target_link_libraries(${TARGET_NAME} PRIVATE 
    ftxui-dom
    ftxui-component
    ftxui-screen
)
endfunction()

project_link_libraries(${PROJECT_NAME})
project_link_libraries(SearchingTool)
#project_link_libraries(SM)

# message(STATUS "Xlnt_INCLUDE_DIRS = ${Xlnt_INCLUDE_DIRS}")
# message(STATUS "xlnt include dirs: $<TARGET_PROPERTY:xlnt::xlnt,INTERFACE_INCLUDE_DIRECTORIES>")


# 根据当前配置选择 DLL
# xlnt的动态链接库地址
set(XLNT_DLL_PATH
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/xlnt/Debug/xlntd.dll>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/xlnt/Release/xlnt.dll>
)
# opencv的动态链接库地址
set(OPENCVWORLD_DLL_PATH
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/opencv/bin/opencv_world4100d.dll>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/opencv/bin/opencv_world4100.dll>
)
set(OPENCV_FFMPEG_DLL_PATH
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/opencv/bin/opencv_videoio_ffmpeg4100_64.dll>
)
set(OPENCV_MSMF_DLL_PATH
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/opencv/bin/opencv_videoio_msmf4100_64.dll>
)
# zlib的动态链接库地址
set(ZLIB_DLL_PATH
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/zlib/Debug/zlibwapi.dll>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/zlib/Release/zlibwapi.dll>
)

# 封装文件复制逻辑为函数：参数为 目标名称（如 SmartSheet 或 SearchingTool）
function(copy_files_to_target TARGET_NAME)
    # -------------------------- 1. 复制 xlnt DLL --------------------------
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${XLNT_DLL_PATH}
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying xlnt dll to ${TARGET_NAME} output directory"
    )

    # -------------------------- 2. 复制 OpenCV DLL --------------------------
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENCVWORLD_DLL_PATH}
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying opencv world dll to ${TARGET_NAME} output directory"
    )
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENCV_FFMPEG_DLL_PATH}
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying opencv ffmpeg dll to ${TARGET_NAME} output directory"
    )
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENCV_MSMF_DLL_PATH}
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying opencv msmf dll to ${TARGET_NAME} output directory"
    )

    # -------------------------- 3. 复制 zlib DLL --------------------------
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${ZLIB_DLL_PATH}
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying zlib dll to ${TARGET_NAME} output directory"
    )

    # -------------------------- 4. 复制 OCR DLL --------------------------
    if(OCR_DLL_RELEASE_FILES)
        foreach(DLL_FILE ${OCR_DLL_RELEASE_FILES})
            get_filename_component(DLL_NAME ${DLL_FILE} NAME)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${DLL_FILE}"
                        $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMENT "Copying ${DLL_NAME} to ${TARGET_NAME} output directory"
                VERBATIM
            )
        endforeach()
    else()
        message(WARNING "No OCR DLL files will be copied to ${TARGET_NAME} - none found")
    endif()

    # -------------------------- 5. 复制 OCR 模型 --------------------------
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/models
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/models
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SOURCE_OCR_MODELS}
                $<TARGET_FILE_DIR:${TARGET_NAME}>/models
        COMMENT "Copying OCR models to ${TARGET_NAME} output directory"
    )

    # -------------------------- 6. 复制 ICU DLL --------------------------
    if(ICU_DLL_RELEASE_FILES)
        foreach(DLL_FILE ${ICU_DLL_RELEASE_FILES})
            get_filename_component(DLL_NAME ${DLL_FILE} NAME)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${DLL_FILE}"
                        $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMENT "Copying ${DLL_NAME} to ${TARGET_NAME} output directory"
                VERBATIM
            )
        endforeach()
    else()
        message(WARNING "No ICU DLL files will be copied to ${TARGET_NAME} - none found")
    endif()

    # -------------------------- 7. 复制 Poppler DLL --------------------------
    if(POPPLER_DLL_RELEASE_FILES)
        foreach(DLL_FILE ${POPPLER_DLL_RELEASE_FILES})
            get_filename_component(DLL_NAME ${DLL_FILE} NAME)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${DLL_FILE}"
                        $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMENT "Copying ${DLL_NAME} to ${TARGET_NAME} output directory"
                VERBATIM
            )
        endforeach()
    else()
        message(WARNING "No Poppler DLL files will be copied to ${TARGET_NAME} - none found")
    endif()
endfunction()

# -------------------------- 关键：为两个目标调用复制函数 --------------------------
copy_files_to_target(${PROJECT_NAME})   # 为 SmartSheet 复制文件
copy_files_to_target(SearchingTool)     # 为 SearchingTool 复制文件
#copy_files_to_target(SM)     # 为 SM 复制文件

########################################Windows下可运行，linux下需要重新写##########################################
########################################Windows下可运行，linux下需要重新写##########################################
########################################Windows下可运行，linux下需要重新写##########################################
########################################Windows下可运行，linux下需要重新写##########################################

elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "当前配置仅支持Windows系统，Linux系统的配置尚未完成，请根据需要自行修改CMakeLists.txt文件")

# macOS  ?????，我不想写了

endif() # windows下可运行，linux下需要重新写
