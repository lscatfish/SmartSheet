
#include <ChineseEncoding.h>
#include <consoleapi2.h>
#include <Files.h>
#include <ios>
#include <iostream>
#include <limits>
#include <minwindef.h>
#include <PersonnelInformation.h>
#include <QingziClass.h>
#include <string>
#include <stringapiset.h>
#include <test.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>

/*
 * @brief 按回车键继续
 */
static void pause( ) {
    std::cout << std::endl;
    std::cout << anycode_to_utf8("请按 Enter 键继续...") << std::endl;
    std::cin.ignore((std::numeric_limits< std::streamsize >::max)( ), '\n');
    // 这里实际上是std::cin.ignore(std::numeric_limits< std::streamsize >::max( ), '\n');
    // 加一个括号防止windows.h的宏污染
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
