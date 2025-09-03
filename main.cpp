
#include <basic.hpp>
#include <console.h>
#include <Encoding.h>
#include <helper.h>
#include <iostream>
#include <pdf.h>
#include <QingziClass.h>
#include <settings.h>
#include <string>
#include <test/test.h>
#include <test/test_for_chstring.h>
#include <test/test_for_ui.h>

int main( ) {

    console::set_console_utf8( );    // 设置控制台为UTF-8编码
    encoding::Init( );

#if true

    pdf::Init( );    // 初始化poppler
    if (!start_warning( )) {
        std::cout << std::endl
                  << U8C(u8"程序结束...");
        pause( );
        return -1;    // 用户选择不继续，程序终止
    }
    settings::set_path( );

    console::clear_console( );
    DoQingziClass *qClass = new DoQingziClass;
    if (!qClass) {
        std::cerr << U8C(u8"青字班模块初始化失败（内存分配失败），程序终止...") << std::endl;
        delete qClass;
        pause( );
        return -1;
    }
    if (!qClass->self_check( )) {
        std::cout << std::endl;
        std::cerr << U8C(u8"青字班模块自检失败，程序终止...") << std::endl;
        delete qClass;
        pause( );
        return -1;
    }

    qClass->start( );
    delete qClass;

#else

    test_for_ManualDocPerspectiveCorrector( );
    pause( );

#endif    // true

    std::cout << std::endl
              << U8C(u8"程序结束...");
    pause( );

    return 0;    // 程序正常结束
}
