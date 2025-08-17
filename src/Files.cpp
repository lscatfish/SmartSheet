
#include <basic.hpp>
#include <Encoding.h>
#include <Files.h>
#include <filesystem>
#include <Fuzzy.h>
#include <helper.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <Windows.h>
#include <xlnt/xlnt.hpp>

namespace file {

// 递归遍历文件夹，收集所有文件路径到 vector 中
void DefFolder::traverse_folder(const std::string &folderPath, list< std::string > &filePaths) {
    // 构建搜索路径（添加通配符*匹配所有项）
    std::string searchPath = folderPath + "\\*";

    // 用于存储搜索结果的结构体
    WIN32_FIND_DATAA findData;
    HANDLE           hFind = FindFirstFileA(searchPath.c_str( ), &findData);

    // 检查搜索是否成功
    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << u8"无法打开文件夹: " << folderPath << std::endl;
        return;
    }

    // 遍历所有找到的项
    do {
        std::string fileName = findData.cFileName;

        // 跳过当前目录(.)、上级目录(..)以及__MACOSX文件夹
        if (fileName == "." || fileName == ".." || fileName == "__MACOSX" || fileName == "_MACOSX") {
            continue;
        }

        // 构建完整路径
        std::string fullPath = folderPath + "\\" + fileName;

        // 判断当前项是否为文件夹
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 若是文件夹，递归遍历
            traverse_folder(fullPath, filePaths);
        } else {
            // 若是文件，添加到路径列表
            filePaths.push_back(fullPath);
        }

    } while (FindNextFileA(hFind, &findData) != 0);    // 继续搜索下一项

    // 关闭搜索句柄
    FindClose(hFind);
}

/*
 * @brief 输出文件夹下的各个文件路径(utf8编码)
 * @return list<string>类型一个列表
 */
list< std::string > DefFolder::get_filePath_list( ) {
    return filePathList_;
}

/*
 * @brief 输出文件夹下的各个文件路径
 * @return list<string>类型一个列表
 */
list< std::string > file::DefFolder::get_u8filePath_list( ) {
    return u8filePathList_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////




/*
 * @brief 解析文件名的后缀与文件名字（不含后缀）
 * @param _input 输入的文件名
 * @return 文件名字（不含后缀）与 后缀 的pair
 */
std::pair< std::string, std::string > separate_filename_and_extension(const std::string &_input) {
    // 找到最后一个 '.' 的位置
    size_t pos = _input.find_last_of('.');
    // 如果没有找到 '.'，返回原字符和空字符串
    if (pos == std::string::npos) {
        return { _input, "" };
    }
    // 返回从字符串开始到 '.' 的子字符串（不包括 '.'）
    std::string part1 = _input.substr(0, pos);
    // 返回从 '.' 开始到字符串末尾的子字符串（包括 '.'）
    std::string part2 = _input.substr(pos);
    return { part1, part2 };
}

/*
 * @brief 从一个文件下获取所有符合后缀条件的文件
 * @param _name 返回的文件名（无格式后缀）
 * @param _path 返回的文件路径
 * @param _foldername 目标文件夹名称
 * @param _extension 文件的后缀集合
 * @return 错误返回false，读取成功返回true
 */
bool get_filepath_from_folder(
    list< std::string >       &_name,
    list< std::string >       &_path,
    std::string                _foldername,
    const list< std::string > &_extension) {

    namespace fs = std::filesystem;

    fs::path            foldern = _foldername;    // 目标目录
    list< std::string > fileName;                 // 文件名(包含后缀)

    try {
        for (const auto &entry : fs::recursive_directory_iterator(foldern)) {
            if (fs::is_regular_file(entry.status( ))) {
                fileName.emplace_back(entry.path( ).filename( ).u8string( ));    // 名字(包含后缀)
                _path.emplace_back(entry.path( ).u8string( ));                   // 路径
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << e.what( ) << '\n';
        return false;
    }

    for (auto &aFileName : fileName) {
        // 文件名/后缀
        auto [a, b] = separate_filename_and_extension(aFileName);
        if (fuzzy::search(_extension, b, fuzzy::LEVEL::High)) {
            // 匹配才加入
            _name.push_back(a);
        }
    }

    std::cout << u8"请确认各班（共" << _name.size( ) << u8"个班）：" << std::endl;
    for (auto &s : _name) {
        std::cout << s << std::endl;
    }
    pause( );    // 等待用户按回车

    return true;
}

/*
 * @brief 为了cv库,从一个文件下获取所有符合后缀条件的img
 * @param _path 返回的文件路径，cv使用
 * @param _u8name 返回的文件名（无格式后缀）
 * @param _u8path 返回的文件路径
 * @param _foldername 目标文件夹名称
 * @param _extension 文件的后缀集合
 * @return 错误返回false，读取成功返回true
 */
bool get_imgpath_from_folder(
    list< std::string >       &_path,
    list< std::string >       &_u8name,
    list< std::string >       &_u8path,
    std::string                _foldername,
    const list< std::string > &_extension) {

    namespace fs = std::filesystem;

    fs::path            foldern = _foldername;    // 目标目录
    list< std::string > u8fileName;               // 文件名(包含后缀)

    try {
        for (const auto &entry : fs::recursive_directory_iterator(foldern)) {
            if (fs::is_regular_file(entry.status( ))) {
                u8fileName.emplace_back(entry.path( ).filename( ).u8string( ));    // 名字(包含后缀)
                _path.emplace_back(entry.path( ).string( ));                       // 路径
                _u8path.emplace_back(entry.path( ).u8string( ));                   // 路径
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << e.what( ) << '\n';
        return false;
    }

    for (auto &aFileName : u8fileName) {
        // 文件名/后缀
        auto [a, b] = separate_filename_and_extension(aFileName);
        if (fuzzy::search(_extension, b, fuzzy::LEVEL::High)) {
            // 匹配才加入
            _u8name.push_back(a);
        }
    }

    std::cout << u8"请确认各图片（共" << _u8name.size( ) << u8"张图片）：" << std::endl;
    for (auto &s : _u8name) {
        std::cout << s << std::endl;
    }
    pause( );    // 等待用户按回车

    return true;
}


/*
 * @brief 用于读取表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径
 */
void load_sheet_from_xlsx(table< std::string > &_sheet, std::string _path) {
    xlnt::workbook wb;
    std::cout << u8"load file: " << _path << std::endl;
    wb.load(_path);
    auto ws = wb.active_sheet( );    // 获取当前激活的工作表（唯一一张）

    // 按行遍历
    for (auto row : ws.rows(false)) {
        // 保存当前行所有单元格文本的临时向量
        list< std::string > aSingleRow;
        // 遍历当前行的每个单元格
        for (auto cell : row) {
            // cell.to_string() 把数字、日期、公式等统一转为字符串
            aSingleRow.push_back(cell.to_string( ));
        }
        _sheet.push_back(aSingleRow);
    }
}

/*
 * @brief 签到表表格的储存
 * @param _sheet 储存表格的二维数组
 * @param _path 文件的路径
 * @param _titleName 表格标题的名称
 */
void save_attSheet_to_xlsx(
    table< std::string > &_sheet,
    std::string           _path,
    std::string           _titleName) {

    // 定义字体
    xlnt::font f;
    f.name(u8"仿宋_GB2312");
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

    size_t maxCol = 1;    // sheet中列的数量
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r) {
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c) {
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).border(b);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).font(f);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).alignment(align);
            // 注意 xlnt 行列从 1 开始
        }
        if (maxCol < _sheet[r].size( )) {
            maxCol = _sheet[r].size( );
        }
    }

    // 列宽
    ws.column_properties(1).width        = 7.92;    // 8字符宽
    ws.column_properties(1).custom_width = true;
    for (int i = 2; i <= maxCol; i++) {
        ws.column_properties(i).width        = 23.92;    // 24字符宽
        ws.column_properties(i).custom_width = true;
    }

    for (std::size_t r = 0; r < _sheet.size( ); ++r) {
        ws.row_properties(r + 1).height        = 24;    // 24pt
        ws.row_properties(r + 1).custom_height = true;
    }

    // 插入表的标题
    xlnt::font f_title;
    f_title.name(u8"宋体");
    f_title.size(24);
    ws.insert_rows(1, 1);    // 插入行

    // 合并单元格
    std::string end_col;
    int         temp = maxCol;
    while (temp > 0) {
        int remainder = (temp - 1) % 26;
        end_col       = char('A' + remainder) + end_col;
        temp          = (temp - 1) / 26;
    }
    // 构建合并单元格的范围字符串
    std::string merRange = "A1:" + end_col + "1";
    ws.merge_cells(merRange);

    ws.row_properties(1).height        = 40;    // 40pt
    ws.row_properties(1).custom_height = true;
    ws.cell("A1").value(_titleName);
    ws.cell("A1").font(f_title);
    ws.cell("A1").alignment(align);

    wb.save(_path);
}

/*
 * @brief 考勤表表格的储存
 * @param _sheet 储存表格的二维数组
 * @param _path 文件的路径
 * @param _titleName 表格标题的名称
 */
void save_sttSheet_to_xlsx(
    const table< std::string > &_sheet,
    std::string                &_path,
    std::string                &_titleName) {
    // 定义字体
    xlnt::font fbody;      // 正文字体
    xlnt::font fheader;    // 表头字体
    xlnt::font ftitle;     // 标题字体
    fbody.name(u8"宋体");
    fheader.name(u8"宋体");
    ftitle.name(u8"方正小标宋简体");
    fbody.size(16);
    fheader.size(16);
    ftitle.size(26);
    fheader.bold(true);
    ftitle.bold(true);

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

    size_t maxCol = 1;    // sheet中列的数量
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r) {
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c) {
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).border(b);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).font(fbody);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).alignment(align);
            // 注意 xlnt 行列从 1 开始
        }
        if (maxCol < _sheet[r].size( )) {
            maxCol = _sheet[r].size( );
        }
    }
    // 修改第一行（表头）的字体
    for (size_t c = 0; c < _sheet[0].size( ); c++) {
        ws.cell(xlnt::cell_reference(c + 1, 1)).border(b);
    }

    // 修改列宽
    for (size_t i = 1; i <= maxCol; i++) {
        ws.column_properties(i).width        = 39.92;    // 40字符宽
        ws.column_properties(i).custom_width = true;
    }
    // 修改行高
    for (size_t r = 1; r <= _sheet.size( ); r++) {
        ws.row_properties(r).height        = 25;    // 25pt
        ws.row_properties(r).custom_height = true;
    }

    // 插入标题
    ws.insert_rows(1, 1);
    // 合并单元格
    std::string end_col;
    int         temp = maxCol;
    while (temp > 0) {
        int remainder = (temp - 1) % 26;
        end_col       = char('A' + remainder) + end_col;
        temp          = (temp - 1) / 26;
    }
    // 构建合并单元格的范围字符串（例如 "A1:D1"）
    std::string merRange = "A1:" + end_col + "1";
    ws.merge_cells(merRange);
    ws.row_properties(1).height        = 45;    // 45pt
    ws.row_properties(1).custom_height = true;
    ws.cell("A1").value(_titleName);
    ws.cell("A1").font(ftitle);
    ws.cell("A1").alignment(align);

    // 保存
    wb.save(_path);
}

/*
 * @brief 保存缓存报名信息到xlsx
 * @param _sheet 表格
 */
void save_signSheet_to_xlsx(const table< std::string > &_sheet) {
    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r)
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c)
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
    wb.save("./sign/sign.xlsx");
}

/*
 * @brief 加载缓存报名信息到xlsx
 * @param _sheet 表格
 */
void load_signSheet_from_xlsx(table< std::string > &_sheet) {
    const std::string path = "./sign/sign.xlsx";
    namespace fs           = std::filesystem;
    // 判断此文件是否存在
    if (!fs::exists(path)) {
        std::cout << std::endl
                  << "\033[43;30mWARNING!!!\033[0m" << std::endl;
        std::cout << u8"\033[43;30m缓存文件 " << path << u8" 不存在" << std::endl;
        std::cout << "WARNING!!!\033[0m" << std::endl
                  << std::endl;
        return;
    }
    xlnt::workbook wb;
    wb.load(path);
    std::cout << std::endl
              << u8"load file: " << path << std::endl;
    auto ws = wb.active_sheet( );

    // 按行遍历
    for (auto row : ws.rows(false)) {
        // 保存当前行所有单元格文本的临时向量
        list< std::string > aSingleRow;
        // 遍历当前行的每个单元格
        for (auto cell : row) {
            // cell.to_string() 把数字、日期、公式等统一转为字符串
            aSingleRow.push_back(cell.to_string( ));
            // std::cout << cell << "   ";
        }
        _sheet.push_back(aSingleRow);
        // std::cout << std::endl;
    }
    pause( );
}

/*
 * @brief 保存尚未搜索到的成员到xlsx
 * @param _sheet 表格
 */
void save_unknownPerSheet_to_xlsx(table< std::string > &_sheet) {
    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r)
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c)
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
    wb.save("./output/unknown.xlsx");
}
}    // namespace file
