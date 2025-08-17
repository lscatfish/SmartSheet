
#include "Files.h"
#include "PersonnelInformation.h"
#include "test.h"
#include <algorithm>
#include <chstring.hpp>
#include <consoleapi2.h>
#include <Encoding.h>
#include <exception>
#include <helper.h>
#include <imgs.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ppocr_API.h>
#include <pugixml.hpp>
#include <string>
#include <stringapiset.h>
#include <uchardet.h>
#include <unzip.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <xlnt/xlnt.hpp>
#include <zlib.h>


// 定义单元格结构体，包含内容和位置信息
struct TableCell {
    std::string content;    // 单元格内容
    int         row;        // 行号（从0开始）
    int         col;        // 列号（从0开始）
};

// 从DOCX中读取指定文件
std::vector< char > read_docx_file(const std::string &docx_path, const std::string &inner_file_path) {

    unzFile zip_file = unzOpen(docx_path.c_str( ));

    if (!zip_file) {
        std::cerr << u8"无法打开DOCX文件: " << docx_path << std::endl;
        return { };
    }

    if (unzLocateFile(zip_file, inner_file_path.c_str( ), 0) != UNZ_OK) {
        std::cerr << u8"DOCX中未找到文件: " << inner_file_path << std::endl;
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

// 解析表格并记录单元格位置
std::vector< std::vector< std::vector< TableCell > > > parse_tables_with_position(const std::vector< char > &xml_data) {
    std::vector< std::vector< std::vector< TableCell > > > all_tables;

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

// 打印带位置信息的表格
void print_tables_with_position(const std::vector< std::vector< std::vector< TableCell > > > &tables) {
    std::cout << "共发现 " << tables.size( ) << " 个表格" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (size_t table_idx = 0; table_idx < tables.size( ); ++table_idx) {
        std::cout << "表格 " << table_idx + 1 << ":" << std::endl;
        for (const auto &row : tables[table_idx]) {
            for (const auto &cell : row) {
                // 输出格式：[行,列]内容
                std::cout << "[" << cell.row << "," << cell.col << "]" << cell.content << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << "----------------------------------------" << std::endl;
    }
}


// 测试解析docx文件的minizip与pugixml
int test_for_docx( ) {
    std::vector< char > xml_data = read_docx_file("列.docx", "word/document.xml");
    if (xml_data.empty( )) {
        return 1;
    }

    auto tables = parse_tables_with_position(xml_data);
    if (tables.empty( )) {
        std::cout << "未找到任何表格" << std::endl;
        return 0;
    }

    print_tables_with_position(tables);
    return 0;
}

void test_main( ) {

    using namespace encoding;

    SetConsoleOutputCP(65001);    // 输出代码页设为 UTF-8
    SetConsoleCP(65001);          // 输入代码页也设为 UTF-8

    /* 1. 载入 Excel 文件 ---------------------------------------------------- */
    xlnt::workbook wb;                        // 创建一个工作簿对象
    wb.load(chcode_to_utf8("123我.xlsx"));    // 将磁盘上的 1.xlsx 加载到内存
    auto ws = wb.active_sheet( );             // 获取当前激活的工作表（第一张）

    /* 2. 在控制台提示用户 --------------------------------------------------- */
    std::cout << "正在处理电子表格..." << std::endl;
    std::cout << "正在创建一个总向量，用于存储整个表格内容..." << std::endl;

    /* 3. 准备二维向量保存整张表 -------------------------------------------- */
    // theWholeSpreadSheet[row][col] 即第 row 行第 col 列的字符串
    table< std::string > theWholeSpreadSheet;

    /* 4. 按行遍历工作表 ----------------------------------------------------- */
    // ws.rows(false) 返回行迭代器，false 表示不缓存，节省内存
    for (auto row : ws.rows(false)) {
        std::cout << "为当前行创建一个新的向量..." << std::endl;

        // 保存当前行所有单元格文本的临时向量
        list< std::string > aSingleRow;

        /* 5. 遍历当前行的每个单元格 ---------------------------------------- */
        for (auto cell : row) {
            std::cout << "将该单元格添加到当前行向量..." << std::endl;
            // cell.to_string() 把数字、日期、公式等统一转为字符串
            aSingleRow.push_back(chcode_to_utf8(cell.to_string( )));
        }

        std::cout << "将该整行添加到总向量..." << std::endl;
        theWholeSpreadSheet.push_back(aSingleRow);
    }

    /* 6. 处理完毕，开始输出 -------------------------------------------------- */
    std::cout << "处理完成！" << std::endl;
    std::cout << "读取向量并将内容逐格打印到屏幕..." << std::endl;

    /* 7. 双重循环打印所有单元格 --------------------------------------------- */
    for (std::size_t rowInt = 0; rowInt < theWholeSpreadSheet.size( ); ++rowInt) {
        for (std::size_t colInt = 0; colInt < theWholeSpreadSheet[rowInt].size( ); ++colInt) {
            // 每个单元格单独占一行输出
            std::cout << theWholeSpreadSheet[rowInt][colInt] << std::endl;
        }
    }

    xlnt::workbook wss;

    wss.active_sheet( ).cell("B6").value(chcode_to_utf8("1中国234"));
    wss.save(chcode_to_utf8("./1我/ou操.xlsx"));


    list< std::string > className_;          // 班级名字
    list< std::string > filePathAndName_;    // 每个xlsx文件的位置
    file::get_filepath_from_folder(className_, filePathAndName_, chcode_to_utf8("./input/all/"), list< std::string >{ ".xlsx" });

    table< std::string > test1 = {
        { chcode_to_utf8("序号"), chcode_to_utf8("姓名"), chcode_to_utf8("学号"), chcode_to_utf8("签到") },
        { chcode_to_utf8("1"), chcode_to_utf8("王二"), chcode_to_utf8("20243546545T"), "" },
        { chcode_to_utf8("2"), chcode_to_utf8("张三"), chcode_to_utf8("324352532423"), "" },
        { chcode_to_utf8("3"), chcode_to_utf8("李四"), chcode_to_utf8("324234"), "" },

    };
    file::save_attSheet_to_xlsx(test1, "test1.xlsx", chcode_to_utf8("测试签到表"));
}

// 测试ENCODING
void test_for_ENCODING( ) {
    namespace ec = encoding;
    list< std::string > str{ "dj", "sdbj", "我i妇女", "987飞机发布会",
                             "****加拿大", "曾经多次uy蒂娜", u8"顶峰那我",
                             "顶峰那我", "图片", "青公班1.jpeg", "青公班1lkjhgfghjkjhgsafdgh",
                             "./input/sign_k/青公班-报名表" };
    for (auto &a : str) {
        std::cout << ec::chcode_to_utf8(a) << std::endl;
    }
    for (auto &a : str) {
        std::cout << ec::gbk_to_utf8(a) << std::endl;
    }
}

// 测试chstring
void test_for_chstring( ) {
    namespace ec = encoding;
    std::vector< chstring > str{ "dj", "sdbj", "我i妇女", "987飞机发布会",
                                 "****加拿大", "曾经多次uy蒂娜", u8"顶峰那我",
                                 "顶峰那我", "图片" };
    for (auto &a : str) {
        std::cout << a << std::endl;
    }
}

// 测试opencv的imread
void test_for_cv_imread( ) {
    std::string path = "./input/att_imgs/青书班1.jpeg";
    cv::Mat     img  = cv::imread(path);
    if (img.empty( )) {
        std::cout << u8"gbk图片 " << path << u8" 打开失败" << std::endl;
        return;
    } else {
        std::cout << u8"gbk图片 " << path << u8" 打开成功!!!!" << std::endl;
    }

    std::string u8path = u8"./input/att_imgs/青书班1.jpeg";
    u8path             = encoding::utf8_to_gbk(u8path);
    cv::Mat u8img      = cv::imread(u8path);
    if (u8img.empty( )) {
        std::cout << u8"u8图片 " << u8path << u8" 打开失败" << std::endl;
        return;
    } else {
        std::cout << u8"u8图片 " << u8path << u8" 打开成功!!!!" << std::endl;
    }
}

// 测试融合sheet函数
void test_for_mergeMultipleSheets( ) {
    table< std::string > sh1, sh2{ { "kndslnj", "dsn", "dic" }, { "dusbc", "cdb", "cdb" } }, sh3{ };
    table< std::string > sh = mergeMultipleSheets(sh1, sh2);
    for (size_t r = 0; r < sh.size( ); r++) {
        for (size_t c = 0; c < sh[r].size( ); c++) {
            std::cout << sh[r][c] << "   ";
        }
        std::cout << std::endl;
    }
}

// 测试ppocr
void test_for_ppocr( ) {
    std::vector< std::vector< ppocr::OCRPredictResult > > out;

    cv::Mat img = cv::imread("1.jpeg");

    ppocr::ocr(out, img, img::_ppocrDir_);

    std::cout << std::endl;
    std::cout << std::endl;

    for (auto &page : out) {
        for (auto &cell : page) {
            std::cout << "(" << cell.box[0][0] << "," << cell.box[0][1] << ")";
            std::cout << "(" << cell.box[1][0] << "," << cell.box[1][1] << ")" << std::endl;
            std::cout << "(" << cell.box[2][0] << "," << cell.box[2][1] << ")";
            std::cout << "(" << cell.box[3][0] << "," << cell.box[3][1] << ")";
            std::cout << cell.text << std::endl;
        }
    }
}

// 测试DefFolder
void test_for_DefFolder( ) {
    file::DefFolder     af("./input");
    list< std::string > p = af.get_u8filePath_list( );
    for (auto& l : p) {
        std::cout << l << std::endl;
    }
}
