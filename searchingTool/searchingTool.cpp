
#include <basic.hpp>
#include <chstring.hpp>
#include <console.h>
#include <Files.h>
#include <high.h>
#include <iostream>
#include <searchingTool/message.hpp>
#include <searchingTool/searchingTool.hpp>
#include <string>

// 输入器
void my_inputer(std::string &inputStr) {
    console::clear_input_buffer( );
    std::cout
        << U8C(u8"输入“$exit$”直接退出；输入“$save$”可以保存所有搜索信息后退出，保存的信息在") + msglogger._SEARCH_LOG_
        << std::endl
        << std::endl;
    std::cout << "Search >>> ";
    std::cin >> inputStr;
}

// 搜索函数实现
bool SearchingTool::search_value(const chstring &_target) {
    bool found = false;
    if (founder(_target, xlsxList_)) {
        found = true;
    }
    if (founder(_target, pdfList_)) {
        found = true;
    }
    if (founder(_target, docxList_)) {
        found = true;
    }

    return found;
};

SearchingTool::SearchingTool( )
    : file::DefFolder(file::_INPUT_DIR_, false) {
    parse_list(xlsxList_, myList< chstring >{ ".xlsx" });
    parse_list(pdfList_, myList< chstring >{ ".pdf", ".PDF" });
    parse_list(docxList_, myList< chstring >{ ".docx", ".DOCX" });
};
