
#include <chstring.hpp>
#include <console.h>
#include <iostream>
#include <searchingTool/searchingTool.hpp>
#include <string>

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
