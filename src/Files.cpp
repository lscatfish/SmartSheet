
#include "ChineseEncoding.h"
#include "Files.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <xlnt/cell/cell_reference.hpp>
#include <xlnt/styles/font.hpp>
#include <xlnt/workbook/workbook.hpp>
#include <xlnt/xlnt.hpp>

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
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName) {
    xlnt::workbook wb;
    wb.load(_pathAndName);
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
        _aSheet.push_back(aSingleRow);
    }
}

/*
 * @brief 表格的储存
 * @param 储存表格的二维数组
 * @param 文件的路径
 * @param 表格的名称
 */
void save_sheet_to_file(
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName,
    std::string                                _titleName) {

    // 定义字体
    xlnt::font f;
    f.name(anycode_to_utf8("仿宋_GB2312"));
    f.size(14);

    // 创建边框样式
    xlnt::border::border_property border_prop;
    border_prop.style(xlnt::border_style::thin);    // 细线边框
    border_prop.color(xlnt::color::black( ));

    // 构造外侧框线
    xlnt::border b;
    b.side(xlnt::border_side::start, border_prop);
    b.side(xlnt::border_side::end, border_prop);
    b.side(xlnt::border_side::top, border_prop);
    b.side(xlnt::border_side::bottom, border_prop);

    // 水平居中 + 垂直居中
    xlnt::alignment align;
    align.horizontal(xlnt::horizontal_alignment::center);
    align.vertical(xlnt::vertical_alignment::center);

    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");
    // 逐行逐列写入
    for (std::size_t r = 0; r < _aSheet.size( ); ++r) {
        for (std::size_t c = 0; c < _aSheet[r].size( ); ++c) {
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_aSheet[r][c]);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).border(b);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).font(f);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).alignment(align);
            // 注意 xlnt 行列从 1 开始
        }
    }

    // 列宽
    ws.column_properties(1).width = 7.92;     // 8字符宽
    ws.column_properties(1).custom_width=true;
    ws.column_properties(2).width = 23.92;    // 24字符宽
    ws.column_properties(2).custom_width = true; 
    ws.column_properties(3).width = 23.92;
    ws.column_properties(3).custom_width = true;
    ws.column_properties(4).width        = 23.92;
    ws.column_properties(4).custom_width = true;
    for (std::size_t r = 0; r < _aSheet.size( ); ++r) {
        ws.row_properties(r + 1).height        = 24;    // 24
        ws.row_properties(r + 1).custom_height = true;
    }

    // 插入表的标题
    xlnt::font f_title;
    f_title.name(anycode_to_utf8("宋体"));
    f_title.size(24);
    ws.insert_rows(1, 1);
    ws.merge_cells("A1:D1");
    ws.row_properties(1).height        = 40;    // 40
    ws.row_properties(1).custom_height = true;
    ws.cell("A1").value(_titleName);
    ws.cell("A1").font(f_title);
    ws.cell("A1").alignment(align);


    wb.save(_pathAndName);
}
