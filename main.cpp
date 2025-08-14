
#include <Encoding.h>
#include <consoleapi2.h>
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
#include <PersonnelInformation.h>
#include <QingziClass.h>
#include <string>
#include <stringapiset.h>
#include <test.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <xlnt/xlnt.hpp>

int main( ) {

    SetConsoleOutputCP(CP_UTF8);    // 输出代码页设为 UTF-8
    SetConsoleCP(CP_UTF8);          // 输入代码页也设为 UTF-8

    // 测试“图片”的编码检测结果
    //std::string     test_str = "姓名";    // 假设源代码以 GBK 保存
    //ENCODING code     = detect_chinese_encoding(test_str);
    //std::cout << anycode_to_utf8("编码检测结果：");
    //switch (code) {
    //    case ENCODING::UTF8: std::cout << "UTF8\n"; break;
    //    case ENCODING::GBK: std::cout << "GBK/GB2312\n"; break;
    //    default: std::cout << "UNKNOWN\n";
    //}

    test_for_uchardet( );
    pause( );

    //// 测试“图片”的编码检测结果
    //test_str =anycode_to_utf8 ("姓名");    // 假设源代码以 GBK 保存
    //code     = detect_chinese_encoding(test_str);
    //std::cout << anycode_to_utf8("编码检测结果：");
    //switch (code) {
    //    case ENCODING::UTF8: std::cout << "UTF8\n"; break;
    //    case ENCODING::GBK: std::cout << "GBK/GB2312\n"; break;
    //    default: std::cout << "UNKNOWN\n";
    //}

    //std::cout << test_str << std::endl;
    //pause( );
    //std::cout << anycode_to_utf8("图片") << std::endl;
    //pause( );

    
#if true
    DoQingziClass qClass;
    qClass.start( );
#else
    test_for__load_sheet_from_img( );
#endif    // true



    std::cout << std::endl
              << u8"程序结束...";
    pause( );
    return 0;    // 程序正常结束
}
