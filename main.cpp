
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
#include <PersonnelInformation.h>
#include <QingziClass.h>
#include <string>
#include <stringapiset.h>
#include <test.h>
#include <test_for_cv.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <xlnt/xlnt.hpp>

int main( ) {

    SetConsoleOutputCP(CP_UTF8);    // 输出代码页设为 UTF-8
    SetConsoleCP(CP_UTF8);          // 输入代码页也设为 UTF-8

#if true
    DoQingziClass qClass;
    qClass.start( );
#else
    test_for_docx( );
    pause( );
    // test_for__load_sheet_from_img( );
#endif    // true

    std::cout << std::endl
              << u8"程序结束...";
    pause( );
    return 0;    // 程序正常结束
}
