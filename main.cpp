
#if true

#include <chstring.hpp>
#include <consoleapi2.h>
#include <Encoding.h>
#include <exception>
#include <Files.h>
#include <filesystem>
#include <fstream>
#include <helper.h>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <minwindef.h>
#include <opencv2/opencv.hpp>
#include <pdf.h>
#include <PersonnelInformation.h>
#include <QingziClass.h>
#include <string>
#include <stringapiset.h>
#include <test/test.h>
#include <test/test_for_poppler.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <xlnt/xlnt.hpp>
#include "basic.hpp"

int main( ) {

    SetConsoleOutputCP(CP_UTF8);    // 输出代码页设为 UTF-8
    SetConsoleCP(CP_UTF8);          // 输入代码页也设为 UTF-8
    encoding::Init( );
    pdf::Init( );    // 初始化poppler

#if false
    DoQingziClass qClass;
    qClass.start( );
#else
    tmain( );
    // test_for_poppler_could_use( );
    pause( );

#endif    // true

    std::cout << std::endl
              << U8C(u8"程序结束...");
    pause( );
    return 0;    // 程序正常结束
}

#else    // 此预处理模块用于测试icu库

#include <iostream>
#include <cstring>
#include <unicode/utypes.h>
#include <unicode/unistr.h>
#include <unicode/ucnv.h>
#include <unicode/uloc.h>

// 检查ICU版本是否匹配
bool checkVersion( ) {
    std::cout << "=== 版本检查 ===" << std::endl;
    std::cout << "ICU 编译版本: " << U_ICU_VERSION << std::endl;
    std::cout << "ICU 数据版本: " << U_ICU_DATA_VERSION << std::endl;

    if (std::strcmp(U_ICU_VERSION, "77.1") != 0) {
        std::cerr << "错误: 检测到ICU版本与77.1不匹配!" << std::endl;
        return false;
    }
    return true;
}

// 测试字符串转换功能
bool testStringConversion( ) {
    std::cout << "\n=== 字符串转换测试 ===" << std::endl;

    // 测试数据: 包含多种语言的UTF-8字符串
    const char *utf8Str = u8"Hello, 世界! こんにちは";
    std::cout << "原始UTF-8字符串: " << utf8Str << std::endl;

    // 转换为Unicode字符串
    icu::UnicodeString ustr(utf8Str, "UTF-8");
    if (ustr.isBogus( )) {
        std::cerr << "错误: 无法创建Unicode字符串" << std::endl;
        return false;
    }

    // 转换回UTF-8验证
    std::string result;
    ustr.toUTF8String(result);
    if (result != utf8Str) {
        std::cerr << "错误: UTF-8转换不一致" << std::endl;
        return false;
    }

    // 测试编码转换为GBK
    UErrorCode  status    = U_ZERO_ERROR;
    UConverter *converter = ucnv_open("GBK", &status);
    if (U_FAILURE(status)) {
        std::cerr << "错误: 无法创建GBK转换器 - " << u_errorName(status) << std::endl;
        return false;
    }

    char    gbkBuffer[256];
    int32_t length = ustr.extract(gbkBuffer, sizeof(gbkBuffer), converter, status);
    ucnv_close(converter);

    if (U_FAILURE(status)) {
        std::cerr << "错误: 转换为GBK失败 - " << u_errorName(status) << std::endl;
        return false;
    }

    std::cout << "成功转换为GBK (字节数: " << length << ")" << std::endl;
    return true;
}

// 测试本地化功能
bool testLocalization( ) {
    std::cout << "\n=== 本地化功能测试 ===" << std::endl;

    // 获取可用的语言环境数量
    int32_t localeCount = uloc_countAvailable( );
    std::cout << "ICU支持的语言环境数量: " << localeCount << std::endl;

    if (localeCount <= 0) {
        std::cerr << "错误: 未检测到可用的语言环境" << std::endl;
        return false;
    }

    // 显示前5个可用的语言环境（适配ICU 77.1的uloc_getAvailable接口）
    std::cout << "部分语言环境: ";
    for (int i = 0; i < 5 && i < localeCount; ++i) {
        // 调用仅接受索引参数的uloc_getAvailable
        const char *locale = uloc_getAvailable(i);
        if (locale) {
            std::cout << locale << " ";
        }
    }
    std::cout << std::endl;

    return true;
}

int main( ) {
    std::cout << "=== ICU 77.1 可用性验证程序 ===" << std::endl;

    // 依次执行各项测试
    bool success = true;

    if (!checkVersion( )) success = false;
    if (!testStringConversion( )) success = false;
    if (!testLocalization( )) success = false;

    // 输出最终结果
    if (success) {
        std::cout << "\n=== 所有测试通过! ICU 77.1 工作正常 ===" << std::endl;
        return 0;
    } else {
        std::cerr << "\n=== 测试失败! 请检查ICU配置 ===" << std::endl;
        return 1;
    }
}



#endif