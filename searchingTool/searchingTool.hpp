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
#include <console.h>
#include <Encoding.h>
#include <Files.h>
#include <Fuzzy.h>
#include <helper.h>
#include <iostream>
#include <pdf.h>
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

/// <summary>
/// 定义文本列表结构体
/// </summary>
template < typename T >
struct TextList {
    std::string sysPath;      // 文件路径（按照系统编码）
    std::string u8Path;       // 文件路径（按照UTF-8编码）
    FileType    fileType;     // 文件类型
    size_t      searchNum;    // 搜索到的单元格数量

    // 判断是否存在值
    // out 输出的结果
    //_target 目标值
    bool is_value_exists(myList< std::string > &_out, const std::string &_target);
};

/// <summary>
/// 针对xlsx的特化版本
/// </summary>
template <>
struct TextList< xlnt::workbook > {
    // excel表格的一个单元格
    struct xlsxCELL {
        std::string sheetName;    // 工作表名称
        std::string address;      // 单元格地址
        std::string value;        // 单元格内容

        xlsxCELL( ) = default;
        xlsxCELL(const std::string &_sheetName,
                 const std::string &_address,
                 const std::string &_value)
            : sheetName(_sheetName), address(_address), value(_value) {};

        bool is_empty( ) const {
            return value.empty( );
        }
    };

    std::string               sysPath;          // 文件路径（按照系统编码）
    std::string               u8Path;           // 文件路径（按照UTF-8编码）
    FileType                  fileType;         // 文件类型
    myList< myTable< xlsxCELL > > sheetList;        // excel表格的所有工作表
    size_t                    searchNum = 0;    // 搜索到的单元格数量

    TextList(const std::string &_sysPath, const std::string &_u8Path)
        : sysPath(_sysPath), u8Path(_u8Path) {
        fileType = FileType::XLSX;
        sheetList.clear( );
        xlnt::workbook wb;
        std::cout << "Parse XLSX file: \"" << u8Path << "\"";
        wb.load(u8Path);
        for (auto ws : wb) {
            myTable< xlsxCELL > sheet;
            std::string       sheetName = ws.title( );
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
    bool is_value_exists(myList< std::string > &_out, const std::string &_target) {
        bool found = false;
        for (const auto &sheet : sheetList) {
            for (const auto &row : sheet) {
                for (const auto &cell : row) {
                    if (!cell.is_empty( )) {
                        if (fuzzy::search_substring(cell.value, _target)) {
                            _out.push_back(
                                "Found \"" + _target + "\" in path: \"" + u8Path + "\"\n"
                                + " -sheet:    \"" + cell.sheetName + "\"\n"
                                + " -position: \"" + cell.address + "\"\n"
                                + " -textual:   " + cell.value);
                            found = true;
                        }
                    }
                }
            }
        }
        return found;
    }
};

/// <summary>
/// 针对docx的特化版本
/// </summary>
template <>
struct TextList< docx::DefDocx > {
    std::string sysPath;     // 文件路径（按照系统编码）
    std::string u8Path;      // 文件路径（按照UTF-8编码）
    FileType    fileType;    // 文件类型
    // size_t      searchNum = 0;    // 搜索到的单元格数量

    myList< myTable< docx::TableCell > > tList;    // docx文件中的表格列表

    TextList(const std::string &_sysPath, const std::string &_u8Path)
        : sysPath(_sysPath), u8Path(_u8Path) {
        fileType = FileType::DOCX;
        docx::DefDocx aDocx(sysPath);
        tList = aDocx.get_table_list( );
    };
    ~TextList( ) = default;

    // 判断是否存在值
    // out 输出的结果
    //_target 目标值
    bool is_value_exists(myList< std::string > &_out, const std::string &_target) {
        bool found = false;
        int  page  = 0;
        for (const auto &myTable : tList) {
            page++;
            for (const auto &row : myTable) {
                for (const auto &cell : row) {
                    if (cell.content.size( ) != 0) {
                        if (fuzzy::search_substring(cell.content, _target)) {
                            _out.push_back(
                                "Found \"" + _target + "\" in path: \"" + u8Path + "\"\n"
                                + U8C(u8" -page:     页 ") + std::to_string(page) + "\n"
                                + U8C(u8" -position: 行 ") + std::to_string(cell.row + 1) + U8C(u8" 列 ") + std::to_string(cell.col + 1) + "\n"
                                + " -textual:   " + cell.content);
                            found = true;
                        }
                    }
                }
            }
        }
        return found;
    }
};

/// <summary>
/// 针对pdf的特化版本
/// </summary>
template <>
struct TextList< pdf::DefPdf > {
    std::string sysPath;     // 文件路径（按照系统编码）
    std::string u8Path;      // 文件路径（按照UTF-8编码）
    FileType    fileType;    // 文件类型

    myList< myList< CELL > > outList;

    TextList(const std::string &_sysPath, const std::string &_u8Path)
        : sysPath(_sysPath), u8Path(_u8Path) {
        pdf::DefPdf a(_u8Path, outList);
        fileType = FileType::PDF;
    };
    ~TextList( ) = default;

    // 判断是否存在值
    // out 输出的结果
    //_target 目标值
    bool is_value_exists(myList< std::string > &_out, const std::string &_target) {
        bool found = false;
        for (size_t i = 0; i < outList.size( ); i++) {
            for (const auto &c : outList[i]) {
                if (!c.text.empty( )) {
                    if (fuzzy::search_substring(c.text, _target)) {
                        _out.push_back(
                            "Found \"" + _target + "\" in path: \"" + u8Path + "\"\n"
                            + U8C(u8" -page:     页 ") + std::to_string(i + 1) + "\n"
                            + " -textual:   " + c.text);
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
    SearchingTool( )
        : file::DefFolder(file::_INPUT_DIR_, false) {
        parse_list(xlsxList_, myList< std::string >{ ".xlsx" });
        parse_list(pdfList_, myList< std::string >{ ".pdf", ".PDF" });
        parse_list(docxList_, myList< std::string >{ ".docx", ".DOCX" });
    };
    ~SearchingTool( ) = default;

    // 公开搜索函数
    bool search_value(myList< std::string > &_out, const std::string &_target);

private:
    myList< TextList< xlnt::workbook > > xlsxList_;
    myList< TextList< pdf::DefPdf > >    pdfList_;
    myList< TextList< docx::DefDocx > >  docxList_;

    // 解析list
    template < typename T >
    void parse_list(myList< T > &_list, const myList< std::string > _ex) {
        myList< std::string > u8PathList  = this->get_u8filepath_list(_ex);
        myList< std::string > sysPathList = this->get_sysfilepath_list(_ex);

        for (size_t i = 0; i < sysPathList.size( ); i++) {
            if constexpr (std::is_same_v< T, TextList< xlnt::workbook > >) {
                console::set_progressBar(sysPathList.size( ), 25, i + 1, 1, 1, U8C(u8"xlsx文件解析"));
            } else if constexpr (std::is_same_v< T, TextList< pdf::DefPdf > >) {
                console::set_progressBar(sysPathList.size( ), 25, i + 1, 1, 1, U8C(u8"pdf文件解析"));
            } else if constexpr (std::is_same_v< T, TextList< docx::DefDocx > >) {
                console::set_progressBar(sysPathList.size( ), 25, i + 1, 1, 1, U8C(u8"docx文件解析"));
            }
            std::cout << std::endl;
            T afile(sysPathList[i], u8PathList[i]);
            console::clear_console_after_line(2);
            _list.push_back(afile);
        }
        // pause( );
        console::clear_console( );
    }

    // 搜索
    template < typename T >
    bool founder(myList< std::string > &_out, const std::string &_target, myList< T > &_list) {
        bool found = false;
        /*for (auto &f : _list) {
            if (f.is_value_exists(_out, _target)) {
                found = true;
            }
        }*/
        for (size_t i = 0; i < _list.size( ); i++) {
            if (_list[i].is_value_exists(_out, _target)) {
                found = true;
            }
            if (fuzzy::search_substring(_list[i].u8Path, _target)) {
                _out.push_back(
                    "Found \"" + _target + "\" in path: \"" + _list[i].u8Path + "\"\n"
                    + " -textual_path:   " + _list[i].u8Path);
                found = true;
            }
        }
        return found;
    }

};

// 输入器
void my_inputer(std::string &inputStr);


#endif    // !SEARCHINGTOOL_HPP
