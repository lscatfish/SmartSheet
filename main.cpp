
#include "ChineseEncoding.h"
#include "Files.h"
#include "PersonnelInformation.h"
#include "QingziClass.h"
#include "test.h"
#include <consoleapi2.h>
#include <iostream>
#include <string>
#include <stringapiset.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <xlnt/xlnt.hpp>
#include <ios>
#include <limits>

/*
* @brief 按回车键继续
*/
static void pause( ) {
    std::cout << std::endl;
    std::cout << anycode_to_utf8("请按 Enter 键继续...") << std::endl;
    std::cin.ignore(std::numeric_limits< std::streamsize >::max( ), '\n');
}

int main( ) {

    SetConsoleOutputCP(65001);    // 输出代码页设为 UTF-8
    SetConsoleCP(65001);          // 输入代码页也设为 UTF-8

#if false
    test_main( );
#endif

    DoQingziClass qClass;
    qClass.start( );

    std::cout << std::endl
              << anycode_to_utf8("程序结束...");
    pause( );
    return 0;    // 程序正常结束
}
