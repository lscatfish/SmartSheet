
#include "ChineseEncoding.h"
#include "Files.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <xlnt/xlnt.hpp>
#include <xlnt/workbook/workbook.hpp>

/*
 * @brief 从一个文件下获取所有的文件
 * @param 返回的文件名（无格式后缀）
 * @param 返回的文件路径
 * @param 目标文件夹名称
 * @return 错误返回false，读取成功返回true
 */
bool get_filepath_from_folder(
    std::vector< std::string > &_name,
    std::vector< std::string > &_path,
    std::string                 _foldername) {
    namespace fs = std::filesystem;

    fs::path folder = _foldername;    // 目标目录

    try {
        for (const auto &entry : fs::recursive_directory_iterator(folder)) {
            if (fs::is_regular_file(entry.status( ))) {
                _name.emplace_back(anycode_to_utf8(entry.path( ).filename( ).string( )));    // 名字
                _path.emplace_back(anycode_to_utf8(entry.path( ).string( )));                // 路径
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << e.what( ) << '\n';
        return false;
    }
    for (auto it = _name.begin( ); it != _name.end( ); it++) {
        it->erase(it->end( ) - 5, it->end( ));
    }
    std::cout << anycode_to_utf8("请确认各班（共") << _name.size( ) << anycode_to_utf8("个班）：") << std::endl;
    for (auto &s : _name) {
        std::cout << s << std::endl;
    }
    std::cout << std::endl
              << anycode_to_utf8("按 Enter 键继续...");
    std::cin.get( );    // 等待用户按回车

    return true;
}

/*
 * @brief 用于读取表格（utf8编码）
 * @param 储存表格的二维数组（按照row，column的形式）
 * @param 文件的路径
 */
void load_sheet_from_file(
    std::vector< std::vector< std::string > > &aSheet,
    std::string                                pathAndName) {
    xlnt::workbook wb;
    wb.load(pathAndName);
    auto ws = wb.active_sheet( );    // 获取当前激活的工作表（唯一一张）

    // 按行遍历
    for (auto row : ws.rows(false)) {
        // 保存当前行所有单元格文本的临时向量
        std::vector< std::string > aSingleRow;
        // 遍历当前行的每个单元格
        for (auto cell : row) {
            // cell.to_string() 把数字、日期、公式等统一转为字符串
            aSingleRow.push_back(anycode_to_utf8(cell.to_string( )));
        }
        aSheet.push_back(aSingleRow);
    }
}