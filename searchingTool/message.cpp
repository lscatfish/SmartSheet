
#include <basic.hpp>
#include <chstring.hpp>
#include <high.h>
#include <iostream>
#include <searchingTool/message.hpp>
#include <variant>


MessageLogger::MessageLogger( ) {
    std::cout << "" << std::endl;
}

//析构函数保存搜索记录
MessageLogger::~MessageLogger( ) {
}

// 获取chstring列表
myList< chstring > MessageLogger::get_tempmsgs_chstring( ) const {
    myList< chstring > out;
    for (const auto &amsg : tempMessageList_) {
        if (const MessageDOCX *p = std::get_if< MessageDOCX >(&amsg)) {
            out.push_back(
                chstring("Found \"") + p->targetWord + "\" in path: \"" + p->filepath + "\"\n"
                + U8C(u8" -page:     页 ") + std::to_string(p->page) + "\n"
                + U8C(u8" -position: 行 ") + std::to_string(p->row) + U8C(u8" 列 ") + std::to_string(p->col) + "\n"
                + " -textual:   " + p->textual);
        } else if (const MessagePDF *p = std::get_if< MessagePDF >(&amsg)) {
            out.push_back(
                chstring("Found \"") + p->targetWord + "\" in path: \"" + p->filepath + "\"\n"
                + U8C(u8" -page:     页 ") + std::to_string(p->page) + "\n"
                + " -textual:   " + p->textual);
        } else if (const MessageXLSX *p = std::get_if< MessageXLSX >(&amsg)) {
            out.push_back(
                chstring("Found \"") + p->targetWord + "\" in path: \"" + p->filepath + "\"\n"
                + " -sheet:    \"" + p->sheetname + "\"\n"
                + " -position: \"" + p->address + "\"\n"
                + " -textual:   " + p->textual);
        } else if (const MessagePath *p = std::get_if< MessagePath >(&amsg)) {
            out.push_back(
                chstring("Found \"") + p->targetWord + "\" in path: \"" + p->filepath + "\"\n"
                + " -textual_path:   " + p->filepath);
        }
    }
    return out;
}

// 输入消息
void MessageLogger::inmsg(const DefMessage &_msg) {
    tempMessageList_.push_back(_msg);
}

// 清理临时消息列表
void MessageLogger::clear_temp_msgs( ) {
    for (const auto &amsg : tempMessageList_)
        permMessageList_.push_back(amsg);
    tempMessageList_.clear( );
}

MessageLogger::MessageXLSX::MessageXLSX(
    const chstring &_filepath,
    const chstring &_targetWord,
    const chstring &_textual,
    const chstring &_address,
    const chstring &_sheetname)
    : filepath(_filepath),
      targetWord(_targetWord),
      textual(_textual),
      address(_address),
      sheetname(_sheetname) {}

MessageLogger::MessageDOCX::MessageDOCX(
    const chstring &_filepath,
    const chstring &_targetWord,
    const chstring &_textual,
    int _page, int _row, int _col)
    : filepath(_filepath),
      targetWord(_targetWord),
      textual(_textual),
      page(_page), row(_row + 1),
      col(_col + 1) {}

MessageLogger::MessagePDF::MessagePDF(
    const chstring &_filepath,
    const chstring &_targetWord,
    const chstring &_textual,
    int             _page)
    : filepath(_filepath),
      targetWord(_targetWord),
      textual(_textual),
      page(_page) {}

MessageLogger::MessagePath::MessagePath(const chstring &_f, const chstring &_t)
    : filepath(_f), targetWord(_t) {}
