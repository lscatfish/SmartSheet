#pragma once
/**************************************************************
 * @file 储存搜索产生的消息
 **************************************************************/
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <basic.hpp>
#include <chstring.hpp>
#include <high.h>
#include <string>
#include <variant>


// 消息记录器
class MessageLogger {
public:
    std::string _SEARCH_LOG_ = "./search_log/";

    // 全局访问点（懒汉式：首次调用时初始化）
    static MessageLogger &getInstance( ) {
        // C++11 后，局部静态变量初始化是线程安全的
        static MessageLogger instance;
        return instance;
    }

    struct MessageXLSX {
        chstring    filepath;      // 文件路径
        chstring    targetWord;    // 搜索目标
        chstring    textual;       // 原文
        chstring    address;       // 网格地址
        chstring    sheetname;     // 工作簿名称
        std::string nowtime;       // 时间
        MessageXLSX(const chstring &_filepath,
                    const chstring &_targetWord,
                    const chstring &_textual,
                    const chstring &_address,
                    const chstring &_sheetname);
    };
    struct MessageDOCX {
        chstring    filepath;      // 文件路径
        chstring    targetWord;    // 搜索目标
        chstring    textual;       // 原文
        std::string nowtime;       // 时间
        int         page;          // 页
        int         row;           // 行
        int         col;           // 列
        MessageDOCX(const chstring &_filepath,
                    const chstring &_targetWord,
                    const chstring &_textual,
                    int _page, int _row, int _col);
    };
    struct MessagePDF {
        chstring    filepath;      // 文件路径
        chstring    targetWord;    // 搜索目标
        chstring    textual;       // 原文
        std::string nowtime;       // 时间
        int         page;          // 页
        MessagePDF(const chstring &_filepath,
                   const chstring &_targetWord,
                   const chstring &_textual,
                   int             _page);
    };
    struct MessagePath {
        chstring    filepath;      // 文件路径
        chstring    targetWord;    // 搜索目标
        std::string nowtime;       // 时间
        MessagePath(const chstring &_f, const chstring &_t);
    };

    using DefMessage = std::variant< MessageXLSX, MessageDOCX, MessagePDF, MessagePath >;

    // 获取chstring列表
    myList< chstring > get_tempmsgs_chstring( ) const;

    MessageLogger( );
    ~MessageLogger( );

    // 输入消息
    void inmsg(const DefMessage &);

    // 清理临时消息列表
    void clear_temp_msgs( );

    // 将当前时间转换为指定格式的字符串（本地时间）
    static std::string get_current_time_string(const std::string &format = "%Y-%m-%d %H:%M:%S");

    // 带毫秒精度的时间转换
    static std::string get_current_time_with_ms( );

private:
    myList< DefMessage > tempMessageList_;    // 临时消息列表
    myList< DefMessage > permMessageList_;    // 永久消息列表

    // 禁用拷贝
    MessageLogger(const MessageLogger &)            = delete;
    MessageLogger &operator=(const MessageLogger &) = delete;

    // 保存sheet
    void save_xlsx(const myTable< std::string > &_sheet);
};

typedef MessageLogger::MessageDOCX mDOCX;
typedef MessageLogger::MessageXLSX mXLSX;
typedef MessageLogger::MessagePDF  mPDF;
typedef MessageLogger::MessagePath mPath;

// 首次调用时才创建实例
#define msglogger MessageLogger::getInstance( )



#endif    // !MESSAGE_HPP
