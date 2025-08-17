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


# 找包，这是可行的
# find_package(OpenCV REQUIRED)


# 这里xlnt包不知道什么原因win没法安装，只能分步了
#set(Xlnt_DIR "")
#find_package(Xlnt REQUIRED)

set(THIRD_PARTY_DLL_DIR "${CMAKE_SOURCE_DIR}/third_party/dll/")
set(THIRD_PARTY_LIB_DIR "${CMAKE_SOURCE_DIR}/third_party/lib/")

include_directories("${CMAKE_SOURCE_DIR}/include/")
include_directories("${THIRD_PARTY_DLL_DIR}/xlnt/include/")
include_directories("${THIRD_PARTY_DLL_DIR}/opencv/include/")
include_directories("${THIRD_PARTY_DLL_DIR}/uchardet/src/")
include_directories("${THIRD_PARTY_DLL_DIR}/uchardet/src/LangModels/")
include_directories("${THIRD_PARTY_DLL_DIR}/zlib/include/")

# 找到 src 文件夹下所有的 .cpp 文件
file(GLOB SOURCES "src/*.cpp" "include/*.h" "include/*.hpp" 
)

###########################################################################################
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
###########################################################################################

# 添加执行文件
add_executable(${PROJECT_NAME} "main.cpp" ${SOURCES})


# 链接lib
# 链接到xlnt
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/xlnt/Debug/xlntd.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/xlnt/Release/xlnt.lib>
)
# 链接到opencv
target_link_libraries(${PROJECT_NAME} PRIVATE 
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/opencv/lib/opencv_world4100d.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/opencv/lib/opencv_world4100.lib>
)
# 链接到uchardet
target_link_libraries(${PROJECT_NAME} PRIVATE 
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/uchardet/Debug/uchardet.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/uchardet/Release/uchardet.lib>
)
# 链接到zlib
target_link_libraries(${PROJECT_NAME} PRIVATE 
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/zlib/Debug/zlibwapi.lib>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/zlib/Release/zlibwapi.lib>
)

# message(STATUS "Xlnt_INCLUDE_DIRS = ${Xlnt_INCLUDE_DIRS}")
# message(STATUS "xlnt include dirs: $<TARGET_PROPERTY:xlnt::xlnt,INTERFACE_INCLUDE_DIRECTORIES>")

# 根据当前配置选择 DLL
# xlnt的动态链接库地址
set(XLNT_DLL_PATH
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/xlnt/Debug/xlntd.dll>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/xlnt/Release/xlnt.dll>
)
# opencv的动态链接库地址
set(OPENCV_DLL_PATH
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/opencv/bin/opencv_world4100d.dll>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/opencv/bin/opencv_world4100.dll>
)
# uchardet的动态链接库地址
set(UCHARDET_DLL_PATH
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/uchardet/Debug/uchardet.dll>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/uchardet/Release/uchardet.dll>
)
# zlib的动态链接库地址
set(ZLIB_DLL_PATH
    $<$<CONFIG:Debug>:${THIRD_PARTY_DLL_DIR}/zlib/Debug/zlibwapi.dll>
    $<$<CONFIG:Release>:${THIRD_PARTY_DLL_DIR}/zlib/Release/zlibwapi.dll>
)

# 把 DLL 拷贝到目标可执行文件目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${XLNT_DLL_PATH}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying xlnt dll to output directory"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCV_DLL_PATH}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying opencv dll to output directory"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${UCHARDET_DLL_PATH}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying uchardet dll to output directory"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ZLIB_DLL_PATH}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying zlib dll to output directory"
)

# 遍历每个 OCR-DLL 文件并复制到目标目录 ： 只在找到文件时执行
if(OCR_DLL_RELEASE_FILES)
    foreach(DLL_FILE ${OCR_DLL_RELEASE_FILES})
        get_filename_component(DLL_NAME ${DLL_FILE} NAME)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL_FILE}"  # 添加引号处理路径中的空格
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Copying ${DLL_NAME} to output directory"
            VERBATIM  # 确保正确处理特殊字符
        )
    endforeach()
else()
    message(WARNING "No OCR DLL files will be copied - none found")
endif()

# 复制ocr所需的模型
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # 1. 如果目标文件夹存在则删除
    COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/models
    # 2. 重新创建目标文件夹
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/models  # 确保目标文件夹存在
    COMMAND ${CMAKE_COMMAND} -E copy_directory                # 复制整个文件夹
        ${SOURCE_OCR_MODELS}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/models
    COMMENT "Copying ${SOURCE_OCR_MODELS} to executable directory"  # 显示提示信息
)