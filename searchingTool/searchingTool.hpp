#pragma once

#ifndef SEARCHINGTOOL_HPP
#define SEARCHINGTOOL_HPP

/*********************************************************************
 *
 * @file searchingTool.hpp
 * @brief 定义搜索工具类
 *
 * @todo 这里的几个模板结构体与模板函数有一点屎山的味道，可以考虑优化一下，使得程序更安全、占用内存更少
 *
 * 作者：lscatfish
 *
 ******************************************************************************/


#include <basic.hpp>
#include <chstring.hpp>
#include <console.h>
#include <Encoding.h>
#include <Files.h>
#include <Fuzzy.h>
#include <helper.h>
#include <high.h>
#include <iostream>
#include <pdf.h>
#include <searchingTool/message.hpp>
#include <string>
#include <type_traits>
#include <vector>
#include <word.h>
#include <xlnt/workbook/workbook.hpp>
#include <xlnt/xlnt.hpp>


// 文件类型枚举
enum class FileType {
    PDF = 0,
    XLSX,
    DOCX,
    IMG
};

// 定义文本列表结构体
template < typename T >
struct TextList {
    chstring path;         // 文件路径（按照系统编码）
    FileType fileType;     // 文件类型
    size_t   searchNum;    // 搜索到的单元格数量

    // 判断是否存在值
    // out 输出的结果
    //_target 目标值
    bool is_value_exists(const chstring &_target);
};

// 针对xlsx的特化版本
template <>
struct TextList< xlnt::workbook > {
    // excel表格的一个单元格
    struct xlsxCELL {
        chstring sheetName;    // 工作表名称
        chstring address;      // 单元格地址
        chstring value;        // 单元格内容

        xlsxCELL( ) = default;
        xlsxCELL(const std::string &_sheetName,
                 const std::string &_address,
                 const std::string &_value)
            : sheetName(_sheetName), address(_address), value(_value) {};

        bool is_empty( ) const {
            return value.empty( );
        }
    };

    chstring                      path;             // 文件路径（按照系统编码）
    FileType                      fileType;         // 文件类型
    myList< myTable< xlsxCELL > > sheetList;        // excel表格的所有工作表
    size_t                        searchNum = 0;    // 搜索到的单元格数量

    TextList(const chstring &_path)
        : path(_path) {
        fileType = FileType::XLSX;
        sheetList.clear( );
        xlnt::workbook wb;
        std::cout << "Parse XLSX file: \"" << path << "\"";
        wb.load(path.u8string( ));
        for (auto ws : wb) {
            myTable< xlsxCELL > sheet;
            std::string         sheetName = ws.title( );
            // 按行遍历
            for (auto row : ws.rows(false)) {
                // 保存当前行所有单元格文本的临时向量
                myList< xlsxCELL > aSingleRow;
                // 遍历当前行的每个单元格
                for (auto cell : row) {
                    // cell.to_string() 把数字、日期、公式等统一转为字符串
                    aSingleRow.push_back(xlsxCELL(sheetName, cell.reference( ).to_string( ), cell.to_string( )));
                    searchNum++;
                }
                sheet.push_back(aSingleRow);
            }
            sheetList.push_back(sheet);
        }
        std::cout << " - Done! Found " << sheetList.size( ) << " sheets." << std::endl;
    };
    ~TextList( ) = default;

    // 判断是否存在值
    // out 输出的结果
    //_target 目标值
    bool is_value_exists(const chstring &_target) {
        bool found = false;
        for (const auto &sheet : sheetList) {
            for (const auto &row : sheet) {
                for (const auto &cell : row) {
                    if (!cell.is_empty( )) {
                        if (cell.value.has_subchstring(_target)) {
                            msglogger.inmsg(mXLSX(path, _target, cell.value, cell.address, cell.sheetName));
                            found = true;
                        }
                    }
                }
            }
        }
        return found;
    }
};

// 针对docx的特化版本
template <>
struct TextList< docx::DefDocx > {
    chstring path;        // 文件路径（按照系统编码）
    FileType fileType;    // 文件类型

    myList< myTable< docx::TableCell > > tList;    // docx文件中的表格列表

    TextList(const chstring &_path)
        : path(_path) {
        fileType = FileType::DOCX;
        docx::DefDocx aDocx(path);
        tList = aDocx.get_table_list( );
    };
    ~TextList( ) = default;

    // 判断是否存在值
    // out 输出的结果
    //_target 目标值
    bool is_value_exists(const chstring &_target) {
        bool found = false;
        int  page  = 0;
        for (const auto &myTable : tList) {
            page++;
            for (const auto &row : myTable) {
                for (const auto &cell : row) {
                    if (cell.content.size( ) != 0) {
                        if (cell.content.has_subchstring(_target)) {
                            msglogger.inmsg(mDOCX(path, _target, cell.content, page, cell.row, cell.col));
                            found = true;
                        }
                    }
                }
            }
        }
        return found;
    }
};

// 针对pdf的特化版本
template <>
struct TextList< pdf::DefPdf > {
    chstring path;        // 文件路径（按照系统编码）
    FileType fileType;    // 文件类型

    myList< myList< CELL > > outList;

    TextList(const chstring &_path)
        : path(_path) {
        pdf::DefPdf a(path, outList);
        fileType = FileType::PDF;
    };
    ~TextList( ) = default;

    // 判断是否存在值
    // out 输出的结果
    //_target 目标值
    bool is_value_exists(const chstring &_target) {
        bool found = false;
        for (size_t i = 0; i < outList.size( ); i++) {
            for (const auto &c : outList[i]) {
                if (!c.text.empty( )) {
                    if (c.text.has_subchstring(_target)) {
                        msglogger.inmsg(mPDF(path, _target, c.text, i + 1));
                        found = true;
                    }
                }
            }
        }
        return found;
    }
};

// 自DefFolder创建的搜索工具类
class SearchingTool : public file::DefFolder {
public:
    // 构造方式
    SearchingTool( );
    ~SearchingTool( ) = default;

    // 全局访问点（懒汉式：首次调用时初始化）
    static SearchingTool &getInstance( );

    // 公开搜索函数
    bool search_value(const chstring &_target);

private:
    myList< TextList< xlnt::workbook > > xlsxList_;
    myList< TextList< pdf::DefPdf > >    pdfList_;
    myList< TextList< docx::DefDocx > >  docxList_;

    // 解析list
    template < typename T >
    void parse_list(myList< T > &_list, const myList< chstring > &_ex) {
        myList< chstring > PathList = this->get_filepath_list(_ex);

        for (size_t i = 0; i < PathList.size( ); i++) {
            if constexpr (std::is_same_v< T, TextList< xlnt::workbook > >) {
                console::set_progressBar(PathList.size( ), 25, i + 1, 1, 1, U8C(u8"xlsx文件解析"));
            } else if constexpr (std::is_same_v< T, TextList< pdf::DefPdf > >) {
                console::set_progressBar(PathList.size( ), 25, i + 1, 1, 1, U8C(u8"pdf文件解析"));
            } else if constexpr (std::is_same_v< T, TextList< docx::DefDocx > >) {
                console::set_progressBar(PathList.size( ), 25, i + 1, 1, 1, U8C(u8"docx文件解析"));
            }
            std::cout << std::endl;
            T afile(PathList[i]);
            console::clear_console_after_line(2);
            _list.push_back(afile);
        }
        // pause( );
        console::clear_console( );
    }

    // 搜索
    template < typename T >
    bool founder(const chstring &_target, myList< T > &_list) {
        bool found = false;
        for (size_t i = 0; i < _list.size( ); i++) {
            if (_list[i].is_value_exists(_target)) {
                found = true;
            }
            if (_list[i].path.has_subchstring(_target)) {
                msglogger.inmsg(mPath(_list[i].path, _target));
                found = true;
            }
        }
        return found;
    }

    SearchingTool(const SearchingTool &)            = delete;
    SearchingTool &operator=(const SearchingTool &) = delete;
};

#define searchingTool SearchingTool::getInstance( )

// 输入器
void my_inputer(std::string &inputStr);


#endif    // !SEARCHINGTOOL_HPP
