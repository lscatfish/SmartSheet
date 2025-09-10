
#include <basic.hpp>
#include <chrono>    // 现代时间库
#include <chstring.hpp>
#include <console.h>
#include <corecrt.h>
#include <ctime>
#include <Files.h>
#include <high.h>
#include <iomanip>
#include <iostream>
#include <searchingTool/message.hpp>
#include <sstream>
#include <string>
#include <variant>
#include <xlnt/xlnt.hpp>

MessageLogger::MessageLogger( ) {
    permMessageList_.reserve(50);
    tempMessageList_.reserve(500);
    file::create_folder_recursive(_SEARCH_LOG_);
    console::clear_console( );
}

// 析构函数保存搜索记录
MessageLogger::~MessageLogger( ) {
    myTable< std::string > sheet = {
        { U8C(u8"搜索目标"), U8C(u8"位置"), U8C(u8"原文"), U8C(u8"文件地址"), U8C(u8"时间") }
    };
    for (const auto &amsg : permMessageList_) {
        myList< std::string > line;
        if (const MessageDOCX *p = std::get_if< MessageDOCX >(&amsg)) {
            line.push_back(p->targetWord.u8string( ));
            line.push_back(U8C(u8"第") + std::to_string(p->page) + U8C(u8"页，第")
                           + std::to_string(p->row) + U8C(u8"行，第") + std::to_string(p->col) + U8C(u8"列"));
            line.push_back(p->textual.u8string( ));
            line.push_back(p->filepath.u8string( ));
            line.push_back(p->nowtime);
        } else if (const MessagePDF *p = std::get_if< MessagePDF >(&amsg)) {
            line.push_back(p->targetWord.u8string( ));
            line.push_back(U8C(u8"第") + std::to_string(p->page) + U8C("页"));
            line.push_back(p->textual.u8string( ));
            line.push_back(p->filepath.u8string( ));
            line.push_back(p->nowtime);
        } else if (const MessageXLSX *p = std::get_if< MessageXLSX >(&amsg)) {
            line.push_back(p->targetWord.u8string( ));
            line.push_back(p->sheetname.u8string( ) + "," + p->address.u8string( ));
            line.push_back(p->textual.u8string( ));
            line.push_back(p->filepath.u8string( ));
            line.push_back(p->nowtime);
        } else if (const MessagePath *p = std::get_if< MessagePath >(&amsg)) {
            line.push_back(p->targetWord.u8string( ));
            line.push_back("-");
            line.push_back(p->filepath.u8string( ));
            line.push_back(p->filepath.u8string( ));
            line.push_back(p->nowtime);
        }
        sheet.push_back(line);
    }
    save_xlsx(sheet);
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

// 将当前时间转换为指定格式的字符串（本地时间）
std::string MessageLogger::get_current_time_string(const std::string &format) {
    // 1. 获取当前系统时间
    auto now = std::chrono::system_clock::now( );

    // 2. 转换为time_t（秒级时间戳）
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // 3. 转换为本地时间的tm结构体
    std::tm local_tm;
#ifdef _WIN32
    // Windows系统使用localtime_s
    localtime_s(&local_tm, &now_time_t);
#else
    // Linux/macOS使用localtime_r（线程安全版本）
    localtime_r(&now_time_t, &local_tm);
#endif
    // 4. 格式化到字符串
    std::stringstream ss;
    ss << std::put_time(&local_tm, format.c_str( ));
    return ss.str( );
}

// 带毫秒精度的时间转换
std::string MessageLogger::get_current_time_with_ms( ) {
    auto now = std::chrono::system_clock::now( );

    // 提取秒级时间
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm     local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &now_time_t);
#else
    localtime_r(&now_time_t, &local_tm);
#endif
    // 提取毫秒部分
    auto ms = std::chrono::duration_cast< std::chrono::milliseconds >(
                  now.time_since_epoch( ) % std::chrono::seconds(1))
                  .count( );

    // 格式化包含毫秒的字符串
    std::stringstream ss;
    ss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S")
       << "." << std::setw(3) << std::setfill('0') << ms;
    return ss.str( );
}

// 保存sheet
void MessageLogger::save_xlsx(const myTable< std::string > &_sheet) {
    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r)
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c)
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
    std::string nowtime = get_current_time_string("%Y%m%d%H%M%S");
    wb.save(_SEARCH_LOG_ + "/" + nowtime + ".xlsx");
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
      sheetname(_sheetname) {
    nowtime = get_current_time_with_ms( );
}

MessageLogger::MessageDOCX::MessageDOCX(
    const chstring &_filepath,
    const chstring &_targetWord,
    const chstring &_textual,
    int _page, int _row, int _col)
    : filepath(_filepath),
      targetWord(_targetWord),
      textual(_textual),
      page(_page), row(_row + 1),
      col(_col + 1) {
    nowtime = get_current_time_with_ms( );
}

MessageLogger::MessagePDF::MessagePDF(
    const chstring &_filepath,
    const chstring &_targetWord,
    const chstring &_textual,
    int             _page)
    : filepath(_filepath),
      targetWord(_targetWord),
      textual(_textual),
      page(_page) {
    nowtime = get_current_time_with_ms( );
}

MessageLogger::MessagePath::MessagePath(const chstring &_f, const chstring &_t)
    : filepath(_f), targetWord(_t) {
    nowtime = get_current_time_with_ms( );
}
