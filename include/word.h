
#pragma once
#ifndef WORD_H
#define WORD_H

/* **************************************************************************************************
 * @brief 此文件用于解析docx文件
 * 
 * **************************************************************************************************/

#include <basic.hpp>
#include <Encoding.h>
#include <iostream>
#include <PersonnelInformation.h>
#include <string>
#include <vector>

// 解析docx的空间
namespace docx {

// 定义单元格结构体，包含内容和位置信息
struct TableCell {
    std::string content;    // 单元格内容
    int         row;        // 行号（从0开始）
    int         col;        // 列号（从0开始）

    TableCell( ) {
        row     = 0;
        col     = 0;
        content = "";
    };
};

// 此类用于解析docx文件
class DefDocx {
public:
    /*
     * @brief 从DOCX中读取指定文件
     * @param _docx_path 要读取的文件的路径
     * @param _inner_file_path docx解压出来之后要读取的文件
     */
    static std::vector< char > read_docx_file(const std::string &_docx_path, const std::string &_inner_file_path);

    /*
     * @brief 解析表格并记录单元格位置
     * @param _xml_data 输入的docx的xml信息
     */
    static list< table< TableCell > > parse_tables_with_position(const std::vector< char > &_xml_data);

    /*
     * @brief 返回一个具有列表中关键值的表
     * @param _u8imp u8编码的搜索表
     * @return table<TableCell>类型的表
     */
    table< TableCell > get_table_with(const list< std::string > &_u8imp);

    /*
     * @brief 返回一个标准人员信息
     * @param _reMeth 报名方式
     */
    DefPerson get_person(const std::string _reMeth);

    // 打印带位置信息的表格
    void print_tables_with_position( );

    /* ======================================================================================================================= */

    /*
     * @brief 标准构造函数，输入一个docx文件的路径（按照系统编码）
     * @param _path docx文件的路径
     */
    DefDocx(const std::string &_path) {
        path_   = _path;
        u8path_ = encoding::sysdcode_to_utf8(_path);

        std::vector< char > xml_data = read_docx_file(path_, "word/document.xml");
        if (xml_data.empty( )) {
            // 空文件或者是错误
            std::cout << u8"docx文件 " << u8path_ << u8" 有概率损坏" << std::endl;
            return;
        }

        // 解析docx中的表格数据
        tableList_ = parse_tables_with_position(xml_data);
        if (tableList_.empty( )) {
            // 未解析到表格
            std::cout << u8"docx文件 " << u8path_ << u8" 无表格" << std::endl;
        }

        keyTable_ = get_table_with(list< std::string >{ u8"姓名", u8"学号" });

        std::cout << u8"docx文件 " << u8path_ << u8" 解析完毕" << std::endl;
    };

    ~DefDocx( ) = default;

private:
    std::string                path_;         // 此文件储存的路径
    std::string                u8path_;       // 此文件储存的路径(u8编码)
    list< table< TableCell > > tableList_;    // 解析出的表格
    table< TableCell >         keyTable_;     // 关键表格
};

}    // namespace docx



#endif    // !WORD_H
