#pragma once

#ifndef SEARCHINGTOOL_HPP
#define SEARCHINGTOOL_HPP

/*********************************************************************
 *
 * @file searchingTool.hpp
 * @brief 定义搜索工具类
 *
 * 作者：lscatfish
 *
 ******************************************************************************/

#include <basic.hpp>
#include <Encoding.h>
#include <Files.h>
#include <Fuzzy.h>
#include <helper.h>
#include <iostream>
#include <pdf.h>
#include <string>
#include <vector>
#include <word.h>
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
    bool is_value_exists(list< std::string > &_out, const std::string &_target);
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
    list< table< xlsxCELL > > sheetList;        // excel表格的所有工作表
    size_t                    searchNum = 0;    // 搜索到的单元格数量

    TextList( ) = default;
    TextList(const std::string &_sysPath, const std::string &_u8Path)
        : sysPath(_sysPath), u8Path(_u8Path) {
        fileType = FileType::XLSX;
        sheetList.clear( );
        xlnt::workbook wb;
        std::cout << "Parse XLSX file: \"" << u8Path << "\"";
        wb.load(u8Path);
        for (const auto &ws : wb) {
            table< xlsxCELL > sheet;
            std::string       sheetName = ws.title( );
            // 按行遍历
            for (const auto &row : ws.rows(false)) {
                // 保存当前行所有单元格文本的临时向量
                list< xlsxCELL > aSingleRow;
                // 遍历当前行的每个单元格
                for (const auto &cell : row) {
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
    bool is_value_exists(list< std::string > &_out, const std::string &_target) {
        bool found = false;
        for (const auto &sheet : sheetList) {
            for (const auto &row : sheet) {
                for (const auto &cell : row) {
                    if (!cell.is_empty( )) {
                        if (fuzzy::search_substring(cell.value, _target)) {
                            _out.push_back("Found \"" + _target + "\" in\n"
                                           + " -path:     \"" + u8Path + "\"\n"
                                           + " -sheet:    \"" + cell.sheetName + "\"\n"
                                           + " -position: \"" + cell.address + "\"\n"
                                           + " -textual:  \"" + cell.value + "\"");
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
struct TextList< docx::DefDocx > : public docx::DefDocx {
    std::string sysPath;     // 文件路径（按照系统编码）
    std::string u8Path;      // 文件路径（按照UTF-8编码）
    FileType    fileType;    // 文件类型
    // size_t      searchNum = 0;    // 搜索到的单元格数量

    TextList(const std::string &_sysPath, const std::string &_u8Path)
        : sysPath(_sysPath), u8Path(_u8Path), docx::DefDocx(_sysPath) {    // 构造及解析
        fileType = FileType::DOCX;
    };
    ~TextList( ) = default;

    // 判断是否存在值
    // out 输出的结果
    //_target 目标值
    bool is_value_exists(list< std::string > &_out, const std::string &_target) {
        bool found = false;
        auto tList = get_table_list( );
        int  page  = 0;
        for (const auto &table : tList) {
            page++;
            for (const auto &row : table) {
                for (const auto &cell : row) {
                    if (cell.content.size( ) != 0) {
                        if (fuzzy::search_substring(cell.content, _target)) {
                            _out.push_back("Found \"" + _target + "\" in\n"
                                           + " -path:     \"" + u8Path + "\"\n"
                                           + U8C(u8" -page:     页 ") + std::to_string(page) + "\n"
                                           + U8C(u8" -position: 行 ") + std::to_string(cell.row) + U8C(u8" 列 ") + std::to_string(cell.col) + "\n"
                                           + " -textual:  \"" + cell.content + "\"");
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
struct TextList< pdf::DefPdf > : public pdf::DefPdf {
    std::string sysPath;     // 文件路径（按照系统编码）
    std::string u8Path;      // 文件路径（按照UTF-8编码）
    FileType    fileType;    // 文件类型

    list< list< CELL > > outList;

    TextList(const std::string &_sysPath, const std::string &_u8Path)
        : sysPath(_sysPath), u8Path(_u8Path), pdf::DefPdf(_u8Path, outList) {
        fileType = FileType::PDF;
    };
    ~TextList( ) = default;

    bool is_value_exists
};

// 自DefFolder创建的搜索工具类
class SearchingTool : public file::DefFolder {
public:
    // 构造方式
    SearchingTool( )
        : file::DefFolder(file::_INPUT_DIR_, true) {



          };
    ~SearchingTool( ) = default;

private:
};


#endif    // !SEARCHINGTOOL_HPP
