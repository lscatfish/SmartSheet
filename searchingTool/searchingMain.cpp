
#include <basic.hpp>
#include <chstring.hpp>
#include <console.h>
#include <Encoding.h>
#include <Files.h>
#include <helper.h>
#include <high.h>
#include <iostream>
#include <pdf.h>
#include <searchingTool/message.hpp>
#include <searchingTool/searchingTool.hpp>
#include <string>
#include <thread>


int main( ) {
    console::set_console_utf8( );    // 设置控制台为UTF-8编码
    encoding::Init( );
    pdf::Init( );    // 初始化poppler

    std::string inputStr = "";
    searchingTool;    // 初始化
    msglogger;        // 初始化

    while (true) {
        console::clear_console( );
        my_inputer(inputStr);
        if (inputStr == "$exit$" || inputStr == "$save$") {
            msglogger.in_deconstruct(inputStr);
            msglogger.~MessageLogger( );
            break;
        }
        std::cout << "Searching for \"" << inputStr << "\" ..." << std::endl
                  << std::endl;
        if (searchingTool.search_value(inputStr)) {
            myList< chstring > out = msglogger.get_tempmsgs_chstring( );
            for (const auto &a : out)
                std::cout << a << std::endl
                          << std::endl;
        } else {
            std::cout << "Not Found!" << std::endl;
        }
        pause( );
        msglogger.clear_temp_msgs( );
    }

    return 0;    // 程序正常结束
}
