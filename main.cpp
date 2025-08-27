
#include <basic.hpp>
#include <Encoding.h>
#include <helper.h>
#include <iostream>
#include <pdf.h>
#include <QingziClass.h>
#include <settings.h>
#include <string>

int main( ) {

    set_console_utf8( );    // 设置控制台为UTF-8编码
    encoding::Init( );
    pdf::Init( );    // 初始化poppler
    if (!start_warning( )) {
        std::cout << std::endl
                  << U8C(u8"程序结束...");
        pause( );
        return -1;    // 用户选择不继续，程序终止
    }
    settings::set_path( );

#if true
    clearConsole( );
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

#else

    file::DefFolder a(file::_OUTPUT_SIGN_QC_PDF_DIR_);
    a.delete_with( );
    pause( );

#endif    // true

    std::cout << std::endl
              << U8C(u8"程序结束...");
    pause( );
    delete qClass;

    return 0;    // 程序正常结束
}
