
#include <basic.hpp>
#include <chstring.hpp>
#include <Encoding.h>
#include <Fuzzy.h>
#include <helper.h>
#include <high.h>
#include <iostream>
#include <PersonnelInformation.h>
#include <pugiconfig.hpp>
#include <pugixml.hpp>
#include <QingziClass.h>
#include <string>
#include <unzip.h>
#include <vector>
#include <word.h>
#include <zip.h>

namespace docx {


TableCell::TableCell( ) {
    row     = 0;
    col     = 0;
    content = "";
}

/*
 * @brief 标准构造函数，输入一个docx文件的路径（按照系统编码）
 * @param _path docx文件的路径
 */
DefDocx::DefDocx(const chstring &_path) {
    path_ = _path;
    std::cout << "Parse DOCX file: \"" << _path << "\"";

    std::vector< char > xml_data = read_docx_file(path_, "word/document.xml");
    if (xml_data.empty( )) {
        // 空文件或者是错误
        std::cout << U8C(u8" 有概率损坏") << std::endl;
        isOK = false;
        return;
    }

    // 解析docx中的表格数据
    tableList_ = parse_tables_with_position(xml_data);
    if (tableList_.empty( )) {
        // 未解析到表格
        std::cout << U8C(u8" 无表格") << std::endl;
        isOK = false;
        return;
    }

    keyTable_ = get_table_with(myList< chstring >{ U8C(u8"姓名"), U8C(u8"学号") });

    std::cout << " - Done! " << std::endl;
    isOK = true;
}

/*
 * @brief 从DOCX中读取指定文件
 * @param _docx_path 要读取的文件的路径
 * @param _inner_file_path docx解压出来之后要读取的文件
 */
std::vector< char > DefDocx::read_docx_file(const chstring &_docx_path, const chstring &_inner_file_path) {

    unzFile zip_file = unzOpen(_docx_path.sysstring( ).c_str( ));

    if (!zip_file) {
        std::cerr << U8C(u8"无法打开DOCX文件: ") << _docx_path << std::endl;
        return { };
    }

    if (unzLocateFile(zip_file, _inner_file_path.u8string( ).c_str( ), 0) != UNZ_OK) {
        std::cerr << U8C(u8"DOCX中未找到文件: ") << _inner_file_path << std::endl;
        unzClose(zip_file);
        return { };
    }

    unz_file_info file_info;
    if (unzGetCurrentFileInfo(zip_file, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK) {
        std::cerr << U8C(u8"获取文件信息失败") << std::endl;
        unzClose(zip_file);
        return { };
    }

    if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
        std::cerr << U8C(u8"打开文件失败") << std::endl;
        unzClose(zip_file);
        return { };
    }

    std::vector< char > buffer(file_info.uncompressed_size);
    int                 bytes_read = unzReadCurrentFile(zip_file, buffer.data( ), buffer.size( ));
    if (bytes_read != static_cast< int >(file_info.uncompressed_size)) {
        std::cerr << U8C(u8"文件读取不完整") << std::endl;
        unzCloseCurrentFile(zip_file);
        unzClose(zip_file);
        return { };
    }

    unzCloseCurrentFile(zip_file);
    unzClose(zip_file);
    return buffer;
}

/*
 * @brief 解析表格并记录单元格位置
 * @param 输入的docx的xml信息
 */
myList< myTable< TableCell > > DefDocx::parse_tables_with_position(const std::vector< char > &_xml_data) {

    myList< myTable< TableCell > > all_tables;

    pugi::xml_document     doc;
    pugi::xml_parse_result result = doc.load_buffer(_xml_data.data( ), _xml_data.size( ));
    if (!result) {
        std::cerr << U8C(u8"XML解析失败: ") << result.description( ) << std::endl;
        return all_tables;
    }

    // 遍历所有表格（<w:tbl>）
    for (auto tbl_node : doc.select_nodes("//w:tbl")) {
        std::vector< std::vector< TableCell > > myTable;
        int                                     row_index = 0;    // 行索引（从0开始）

        // 遍历表格行（<w:tr>）
        for (auto tr_node : tbl_node.node( ).select_nodes("w:tr")) {
            std::vector< TableCell > row_cells;
            int                      col_index = 0;    // 列索引（从0开始）

            // 遍历单元格（<w:tc>）
            for (auto tc_node : tr_node.node( ).select_nodes("w:tc")) {
                // 提取单元格文本
                std::string cell_text;
                for (auto t_node : tc_node.node( ).select_nodes(".//w:t")) {
                    cell_text += t_node.node( ).text( ).as_string( );
                }

                // 记录位置信息
                TableCell cell;
                cell.content = cell_text;
                cell.row     = row_index;
                cell.col     = col_index;
                cell.content.trim_whitespace( );

                row_cells.push_back(cell);
                col_index++;    // 列索引自增
            }

            myTable.push_back(row_cells);
            row_index++;    // 行索引自增
        }
        all_tables.push_back(myTable);
    }
    return all_tables;
}

// 打印带位置信息的表格
void DefDocx::print_tables_with_position( ) {
    std::cout << U8C(u8"docx文件 ")
              << path_ << U8C(u8" 共发现 ")
              << tableList_.size( ) << U8C(u8" 个表格") << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (size_t table_idx = 0; table_idx < tableList_.size( ); ++table_idx) {
        std::cout << U8C(u8"表格 ") << table_idx + 1 << ":" << std::endl;
        for (const auto &row : tableList_[table_idx]) {
            for (const auto &cell : row) {
                // 输出格式：[行,列]内容
                std::cout << "[" << cell.row << "," << cell.col << "]" << cell.content << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << "----------------------------------------" << std::endl;
    }
}

// 是否解析成功
bool DefDocx::ifOK( ) const {
    return isOK;
}

/*
 * @brief 返回一个具有列表中关键值的表
 * @param _u8imp u8编码的搜索表
 * @return table<TableCell>类型的表
 */
myTable< TableCell > DefDocx::get_table_with(const myList< chstring > &_u8imp) {
    for (const auto &t : this->tableList_) {
        // 构造搜索库
        myList< chstring > searchingLib;
        for (const auto &r : t) {
            for (const auto &c : r) {
                if (c.content.size( ) != 0) {
                    searchingLib.push_back(c.content);
                }
            }
        }
        if (searchingLib.size( ) == 0) {
            continue;
        }
        bool next_table = false;    // 是否检测下一张表
        // 检测关键值
        for (size_t i = 0; i < _u8imp.size( ); i++) {
            if (!fuzzy::search(searchingLib, _u8imp[i], fuzzy::LEVEL::High)) {
                next_table = true;
                break;
            }
        }
        if (!next_table) {
            return t;
        }
    }
    return myTable< TableCell >( );    // 返回空表
}

/*
 * @brief 返回一个标准人员信息
 * @param _reMeth 报名方式
 */
DefPerson DefDocx::get_person( ) {
    DefLine perLine;
    perLine.information[U8C(u8"备注")]     = "";
    perLine.information[U8C(u8"个人特长")] = "";
    perLine.information[U8C(u8"工作经历")] = "";
    perLine.information[U8C(u8"获奖情况")] = "";
    perLine.information[U8C(u8"个人简介")] = "";
    DefPerson per;
    // 定义关键的词
    const myList< chstring > headerLib{
        U8C(u8"姓名"),
        U8C(u8"性别"), U8C(u8"年级"),
        U8C(u8"学号"), U8C(u8"政治面貌"),
        U8C(u8"学院"), U8C(u8"专业"),
        U8C(u8"电话"), U8C(u8"联系方式"),
        U8C(u8"联系电话"), U8C(u8"电话号码"),
        U8C(u8"QQ号"), U8C(u8"qq号"),
        U8C(u8"qq"), U8C(u8"QQ"),
        U8C(u8"所任职务"), U8C(u8"职务"),
        U8C(u8"学生职务"),
        U8C(u8"邮箱"), U8C(u8"民族"),
        U8C(u8"社团"), U8C(u8"报名青字班"),
        U8C(u8"青字班")
    };
    for (size_t row = 0; row < keyTable_.size( ); row++) {
        for (size_t col = 0; col < keyTable_[row].size( ); col++) {
            if (keyTable_[row][col].content.size( ) != 0) {
                if (fuzzy::search(headerLib, keyTable_[row][col].content, fuzzy::LEVEL::High)) {
                    if (col + 1 < keyTable_[row].size( )) {
                        perLine.information[keyTable_[row][col].content] = keyTable_[row][col + 1].content;
                        col++;
                    }
                } else if (keyTable_[row][col].content |= U8C(u8"个人简介")) {
                    if (col + 1 < keyTable_[row].size( )) {
                        if (keyTable_[row][col + 1].content.size( ) != 0)
                            perLine.information[U8C(u8"个人简介")] = keyTable_[row][col + 1].content;
                        if (keyTable_[row][col + 1].content.size( ) < 60) {    // 20字
                            perLine.information[U8C(u8"备注")] += U8C(u8"个人简介少于20字；");
                            col++;
                        } else if (keyTable_[row][col + 1].content.size( ) < 90) {    // 50字
                            perLine.information[U8C(u8"备注")] += U8C(u8"个人简介少于30字；");
                            col++;
                        }
                    }
                } else if (keyTable_[row][col].content |= U8C(u8"个人特长")) {
                    if (col + 1 < keyTable_[row].size( )) {
                        if (keyTable_[row][col + 1].content.size( ) != 0)
                            perLine.information[U8C(u8"个人特长")] = keyTable_[row][col + 1].content;
                        if (keyTable_[row][col + 1].content.size( ) < 30) {    // 10字
                            perLine.information[U8C(u8"备注")] += +U8C(u8"个人特长少于10字；");
                            col++;
                        } else if (keyTable_[row][col + 1].content.size( ) < 60) {    // 20字
                            perLine.information[U8C(u8"备注")] += U8C(u8"个人特长少于20字；");
                            col++;
                        }
                    }
                } else if (keyTable_[row][col].content |= U8C(u8"工作经历")) {
                    if (col + 1 < keyTable_[row].size( )) {
                        if (keyTable_[row][col + 1].content.size( ) != 0)
                            perLine.information[U8C(u8"工作经历")] = keyTable_[row][col + 1].content;
                        if (keyTable_[row][col + 1].content.size( ) < 30) {    // 10字
                            perLine.information[U8C(u8"备注")] += U8C(u8"工作经历少于10字；");
                            col++;
                        } else if (keyTable_[row][col + 1].content.size( ) < 60) {    // 20字
                            perLine.information[U8C(u8"备注")] += U8C(u8"工作经历少于20字；");
                            col++;
                        }
                    }
                } else if (keyTable_[row][col].content |= U8C(u8"获奖情况")) {
                    if (col + 1 < keyTable_[row].size( )) {
                        if (keyTable_[row][col + 1].content.size( ) != 0)
                            perLine.information[U8C(u8"获奖情况")] = keyTable_[row][col + 1].content;
                        col++;
                    }
                }
            }
        }
    }
    DoQingziClass::trans_personline_to_person(perLine, per);
    if (path_.has_subchstring(U8C(u8"自主报名")))
        per.otherInformation[U8C(u8"报名方式")] = U8C(u8"自主报名");
    else if (path_.has_subchstring(U8C(u8"重庆大学团校")))
        per.otherInformation[U8C(u8"报名方式")] = U8C(u8"自主报名");
    else
        per.otherInformation[U8C(u8"报名方式")] = U8C(u8"组织推荐");
    per.otherInformation[U8C(u8"文件地址")] = path_;
    per.optimize( );
    return per;
}

// 返回解析出的表格列表
myList< myTable< TableCell > > DefDocx::get_table_list( ) const {
    return tableList_;
}


}    // namespace docx
