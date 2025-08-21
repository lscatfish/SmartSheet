#include <algorithm>
#include <cmath>
#include <consoleapi2.h>
#include <fstream>
#include <iostream>
#include <string>
#include <test/test_for_pdfium.h>
#include <vector>
#include <WinNls.h>

#include <fpdfview.h>    // PDFium核心头文件

// 验证PDFium安装是否成功
bool checkPdfiumInstallation(const std::string &testPdfPath = u8"列.pdf") {
    // 1. 初始化PDFium库
    FPDF_LIBRARY_CONFIG config;
    config.version          = 2;          // 必须设置为2（当前版本要求）
    config.m_pUserFontPaths = nullptr;    // 使用默认字体路径
    config.m_pIsolate       = nullptr;    // 不使用V8隔离环境
    config.m_v8EmbedderSlot = 0;          // V8相关，不启用时设为0

    FPDF_InitLibraryWithConfig(&config);

    bool isSuccess = false;

    try {
        // 2. 尝试加载测试PDF文件（如果未指定路径，默认尝试当前目录的test.pdf）
        FPDF_DOCUMENT doc = FPDF_LoadDocument(testPdfPath.c_str( ), nullptr);
        if (!doc) {
            unsigned long errorCode = FPDF_GetLastError( );
            std::cerr << u8"错误：加载PDF文件失败，错误码：" << errorCode << std::endl;
            std::cerr << u8"可能原因：文件不存在、路径错误或文件损坏" << std::endl;
        } else {
            // 3. 获取并打印页面数量（验证核心功能）
            int pageCount = FPDF_GetPageCount(doc);
            std::cout << u8"PDFium工作正常！" << std::endl;
            std::cout << u8"成功加载测试文件：" << testPdfPath << std::endl;
            std::cout << u8"文件页数：" << pageCount << std::endl;
            isSuccess = true;

            // 关闭文档
            FPDF_CloseDocument(doc);
        }
    } catch (...) {
        std::cerr << u8"错误：执行过程中发生未知异常" << std::endl;
    }

    // 4. 清理并释放库资源
    FPDF_DestroyLibrary( );
    return isSuccess;
}

// 示例主函数
void test_for_pdfium_can_using( ) {
    // 可以替换为你的测试PDF路径，必须使用u8的路径，逆天又不逆天
    std::string testPdf = u8"测.pdf";

    // 调用验证函数
    bool result = checkPdfiumInstallation(testPdf);
}