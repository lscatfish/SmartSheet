
#include <basic.hpp>
#include <console.h>
#include <Encoding.h>
#include <ErrorHandler/BaseException.hpp>
#include <ErrorHandler/ErrorHandler.hpp>
#include <excel.h>
#include <helper.h>
#include <iostream>
#include <pdf.h>
#include <QingziClass.h>
#include <settings.h>
#include <string>
#include <test/test.h>
#include <test/test_for_chstring.h>
#include <test/test_for_ui.h>

// 开始前警告
bool start_warning( );

int main( ) {
    console::set_console_utf8( );    // 设置控制台为UTF-8编码
    //xlsx::set_alignment( );
#if true
    TRYANY(
        encoding::Init( );
        pdf::Init( );    // 初始化poppler
        if (!start_warning( )) {
            std::cout << std::endl
                      << U8C(u8"程序结束...");
            pause( );
            return -1;    // 用户选择不继续，程序终止
        } settings::set_path( );

        console::clear_console( );
        DoQingziClass *qClass = new DoQingziClass;
        if (!qClass) {
            std::cerr << U8C(u8"青字班模块初始化失败（内存分配失败），程序终止...") << std::endl;
            delete qClass;
            pause( );
            return -1;
        } if (!qClass->self_check( )) {
            std::cout << std::endl;
            std::cerr << U8C(u8"青字班模块自检失败，程序终止...") << std::endl;
            delete qClass;
            pause( );
            return -1;
        }

        qClass->start( );
        delete qClass;)
    CATCH_DOCX_ERROR( )
    CATCH_PDF_ERROR( )
    CATCH_STD_ERROR( )
    CATCH_UNKNOWN_ERROR( )

#else

    test_for_ManualDocPerspectiveCorrector( );
    pause( );

#endif    // true

    std::cout << std::endl
              << U8C(u8"程序结束...");
    pause( );

    return 0;    // 程序正常结束
}


// 开始前警告
bool start_warning( ) {
    console::clear_console( );
    std::cout << U8C(u8"请确保已经关闭工作区（input、output与storage文件夹下所有文件都必须关闭）!!!") << std::endl;
    std::cout << std::endl
              << U8C(u8"-程序运行过程中会在output文件夹内生成结果，请勿删除output文件夹!!!") << std::endl;
    std::cout << U8C(u8"-如果output文件夹内有旧的结果，程序会自动覆盖，请注意备份重要数据!!!") << std::endl;
    std::cout << std::endl
              << U8C(u8"详细的使用教程请参看本程序同目录下的“使用说明”文件") << std::endl;
    std::cout << std::endl
              << U8C(u8"你是否已确认关闭工作区  [Y/n]  （请输入Y以开始程序）:") << std::endl;
    console::clear_input_buffer( );
    std::string yn;
    std::cin >> yn;
    if (yn != "Y" && yn != "y") {
        std::cout << U8C(u8"你未输入Y，程序终止...") << std::endl;
        pause( );
        return false;
    }
    console::clear_input_buffer( );
    return true;
}