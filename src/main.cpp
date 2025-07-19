
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


int main( ) {

    SetConsoleOutputCP(65001);    // 输出代码页设为 UTF-8
    SetConsoleCP(65001);          // 输入代码页也设为 UTF-8

#if false
    test_main( );
#endif
    DoQingziClass qClass;
    qClass.start( );


    return 0;    // 程序正常结束
}
