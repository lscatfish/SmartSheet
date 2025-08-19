#pragma once

#ifndef DLL_H
#define DLL_H

#include <consoleapi2.h>         // SetConsoleOutputCP / SetConsoleCP
#include <iostream>              // 标准输入输出
#include <libloaderapi.h>        // LoadLibrary / GetProcAddress
#include <opencv2/opencv.hpp>    // OpenCV 头文件
#include <string>                // 字符串
#include <vector>                // STL 容器
#include <Windows.h>             // Windows API


// 用于管理手动调用的dll空间
namespace dll {

/*========================================================
 * 4. 工具函数：获取 DLL 导出函数地址
 *    模板函数，支持任意返回类型
 *--------------------------------------------------------*/
template < typename FuncPtr >
bool GetExport(HMODULE h, const char *procName, FuncPtr &outFn) {
    outFn = reinterpret_cast< FuncPtr >(::GetProcAddress(h, procName));
    if (!outFn) {
        std::cerr << "GetProcAddress failed for " << procName
                  << ", error: " << GetLastError( ) << std::endl;
        return false;
    }
    return true;
}

/*========================================================
 * 3. 工具函数：加载 DLL
 *    参数：DLL 的完整路径（宽字符）
 *    返回：模块句柄；失败返回 nullptr
 *--------------------------------------------------------*/
HMODULE LoadDll(const std::wstring &dllPath);

}    // namespace dll



#endif    // !DLL_H
