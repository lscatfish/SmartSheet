
#include <basic.hpp>
#include <chstring.hpp>
#include <console.h>
#include <Encoding.h>
#include <Files.h>
#include <helper.h>
#include <high.h>
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
