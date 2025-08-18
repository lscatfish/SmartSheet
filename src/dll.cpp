#include <consoleapi2.h>    // SetConsoleOutputCP / SetConsoleCP
#include <dll.h>
#include <iostream>              // 标准输入输出
#include <libloaderapi.h>        // LoadLibrary / GetProcAddress
#include <opencv2/opencv.hpp>    // OpenCV 头文件
#include <string>                // 字符串
#include <vector>                // STL 容器
#include <Windows.h>             // Windows API



namespace dll {

/*========================================================
 *    工具函数：加载 DLL
 *    参数：DLL 的完整路径（宽字符）
 *    返回：模块句柄；失败返回 nullptr
 *--------------------------------------------------------*/
HMODULE LoadDll(const std::wstring &dllPath) {
    HMODULE h = ::LoadLibraryW(dllPath.c_str( ));    // 加载 DLL
    if (!h) {
        std::cerr << "LoadLibrary failed: " << GetLastError( )
                  << "  DLL path: " << std::string(dllPath.begin( ), dllPath.end( )) << std::endl;
    }
    return h;
}


}    // namespace dll
