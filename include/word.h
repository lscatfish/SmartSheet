
#pragma once
#ifndef WORD_H
#define WORD_H

/* **************************************************************************************************
 * @brief 此文件用于解析docx文件
 *
 * **************************************************************************************************/

#include <basic.hpp>
#include <string>
#include <vector>

// 解析docx的空间
namespace docx {

// 定义单元格结构体，包含内容和位置信息
struct TableCell {
    std::string content;    // 单元格内容
    int         row;        // 行号（从0开始）
    int         col;        // 列号（从0开始）
};

// 此类用于解析docx文件
class DefDocx {
public:
    DefDocx( ) {};
    ~DefDocx( ) = default;

    /*
     * @brief 从DOCX中读取指定文件
     * @param docx_path 要读取的文件的路径
     * @param inner_file_path docx解压出来之后要读取的文件
     */
    static std::vector< char > read_docx_file(const std::string &docx_path, const std::string &inner_file_path);

    /*
     * @brief 解析表格并记录单元格位置
     * @param 输入的docx的xml信息
     */
    static list< table< TableCell > > parse_tables_with_position(const std::vector< char > &xml_data);

private:
    std::string path_;    // 此文件储存的路径
};

}    // namespace docx



#endif    // !WORD_H
