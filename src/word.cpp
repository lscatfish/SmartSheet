
#include <basic.hpp>
#include <unzip.h>
#include <word.h>
#include <zip.h>
#include<pugixml.hpp>

namespace docx {

/*
 * @brief 从DOCX中读取指定文件
 * @param _docx_path 要读取的文件的路径
 * @param _inner_file_path docx解压出来之后要读取的文件
 */
std::vector< char > DefDocx::read_docx_file(const std::string &_docx_path, const std::string &_inner_file_path) {

    unzFile zip_file = unzOpen(_docx_path.c_str( ));

    if (!zip_file) {
        std::cerr << u8"无法打开DOCX文件: " << _docx_path << std::endl;
        return { };
    }

    if (unzLocateFile(zip_file, _inner_file_path.c_str( ), 0) != UNZ_OK) {
        std::cerr << u8"DOCX中未找到文件: " << _inner_file_path << std::endl;
        unzClose(zip_file);
        return { };
    }

    unz_file_info file_info;
    if (unzGetCurrentFileInfo(zip_file, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK) {
        std::cerr << u8"获取文件信息失败" << std::endl;
        unzClose(zip_file);
        return { };
    }

    if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
        std::cerr << u8"打开文件失败" << std::endl;
        unzClose(zip_file);
        return { };
    }

    std::vector< char > buffer(file_info.uncompressed_size);
    int                 bytes_read = unzReadCurrentFile(zip_file, buffer.data( ), buffer.size( ));
    if (bytes_read != static_cast< int >(file_info.uncompressed_size)) {
        std::cerr << u8"文件读取不完整" << std::endl;
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
list< table< TableCell > > docx::DefDocx::parse_tables_with_position(const std::vector< char > &xml_data) {

    list< table< TableCell > > all_tables;

    pugi::xml_document     doc;
    pugi::xml_parse_result result = doc.load_buffer(xml_data.data( ), xml_data.size( ));
    if (!result) {
        std::cerr << u8"XML解析失败: " << result.description( ) << std::endl;
        return all_tables;
    }

    // 遍历所有表格（<w:tbl>）
    for (auto tbl_node : doc.select_nodes("//w:tbl")) {
        std::vector< std::vector< TableCell > > table;
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

                row_cells.push_back(cell);
                col_index++;    // 列索引自增
            }

            table.push_back(row_cells);
            row_index++;    // 行索引自增
        }

        all_tables.push_back(table);
    }

    return all_tables;
}



}    // namespace docx
