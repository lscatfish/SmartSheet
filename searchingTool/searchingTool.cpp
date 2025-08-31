
#include <basic.hpp>
#include <console.h>
#include <Encoding.h>
#include <Files.h>
#include <helper.h>
#include <iostream>
#include <pdf.h>
#include <searchingTool/searchingTool.hpp>
#include <string>
#include <xlnt/workbook/workbook.hpp>

// 输入器
void my_inputer(std::string &inputStr) {
    console::clear_input_buffer( );
    std::cout << std::endl;
    std::cout << "Search >>> ";
    std::cin >> inputStr;
}

// 搜索函数实现
bool SearchingTool::search_value(list< std::string > &_out, const std::string &_target) {
    bool found = false;
    if (founder(_out, _target, xlsxList_)) {
        found = true;
    }
    if (founder(_out, _target, pdfList_)) {
        found = true;
    }
    if (founder(_out, _target, docxList_)) {
        found = true;
    }

    return found;
};
