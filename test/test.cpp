
#include "Files.h"
#include "PersonnelInformation.h"
#include <algorithm>
#include <basic.hpp>
#include <chstring.hpp>
#include <consoleapi2.h>
#include <Encoding.h>
#include <exception>
#include <helper.h>
#include <icu_encoding_handler.h>
#include <imgs.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ppocr_API.h>
#include <pugixml.hpp>
#include <string>
#include <stringapiset.h>
#include <test/test.h>
#include <unzip.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <word.h>
#include <xlnt/xlnt.hpp>
#include <zlib.h>


// 测试解析docx文件的minizip与pugixml
int test_for_docx( ) {

    docx::DefDocx d("./列.docx");
    d.print_tables_with_position( );


    auto tables = d.get_table_with(list< std::string >{ U8C(u8"姓名"), U8C(u8"性别") });
    if (tables.empty( )) {
        std::cout << U8C(u8"未找到任何表格") << std::endl;
        return 0;
    }

    return 0;
}

void test_main( ) {

    using namespace encoding;

    SetConsoleOutputCP(65001);    // 输出代码页设为 UTF-8
    SetConsoleCP(65001);          // 输入代码页也设为 UTF-8

    /* 1. 载入 Excel 文件 ---------------------------------------------------- */
    xlnt::workbook wb;                          // 创建一个工作簿对象
    wb.load(sysdcode_to_utf8("123我.xlsx"));    // 将磁盘上的 1.xlsx 加载到内存
    auto ws = wb.active_sheet( );               // 获取当前激活的工作表（第一张）

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
            aSingleRow.push_back(sysdcode_to_utf8(cell.to_string( )));
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

    wss.active_sheet( ).cell("B6").value(sysdcode_to_utf8("1中国234"));
    wss.save(sysdcode_to_utf8("./1我/ou操.xlsx"));

    list< std::string > className_;          // 班级名字
    list< std::string > filePathAndName_;    // 每个xlsx文件的位置
    file::get_filepath_from_folder(className_, filePathAndName_, sysdcode_to_utf8("./input/all/"), list< std::string >{ ".xlsx" });

    table< std::string > test1 = {
        { sysdcode_to_utf8("序号"), sysdcode_to_utf8("姓名"), sysdcode_to_utf8("学号"), sysdcode_to_utf8("签到") },
        { sysdcode_to_utf8("1"), sysdcode_to_utf8("王二"), sysdcode_to_utf8("20243546545T"), "" },
        { sysdcode_to_utf8("2"), sysdcode_to_utf8("张三"), sysdcode_to_utf8("324352532423"), "" },
        { sysdcode_to_utf8("3"), sysdcode_to_utf8("李四"), sysdcode_to_utf8("324234"), "" },

    };
    file::save_attSheet_to_xlsx(test1, "test1.xlsx", U8C(u8"测试签到表"));
}

// 测试ENCODING
void test_for_ENCODING( ) {
    namespace ec = encoding;
    list< std::string > str{ "dj", "sdbj", "我i妇女", "987飞机发布会",
                             "****加拿大", "曾经多次uy蒂娜", U8C(u8"顶峰那我"),
                             "顶峰那我", "图片", "青公班1.jpeg", "青公班1lkjhgfghjkjhgsafdgh",
                             "./input/sign_k/青公班-报名表" };
    for (auto &a : str) {
        std::cout << ec::sysdcode_to_utf8(a) << std::endl;
    }
}

// 测试chstring
void test_for_chstring( ) {
    namespace ec = encoding;
    std::vector< chstring > str{ "dj", "sdbj", "我i妇女", "987飞机发布会",
                                 "****加拿大", "曾经多次uy蒂娜", U8C(u8"顶峰那我"),
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
        std::cout << U8C(u8"gbk图片 ")
                  << path << U8C(u8" 打开失败") << std::endl;
        return;
    } else {
        std::cout << U8C(u8"gbk图片 ")
                  << path << U8C(u8" 打开成功!!!!") << std::endl;
    }

    std::string u8path = U8C(u8"./input/att_imgs/青书班1.jpeg");
    // u8path             = encoding::utf8_to_gbk(u8path);
    cv::Mat u8img = cv::imread(u8path);
    if (u8img.empty( )) {
        std::cout << U8C(u8"u8图片 ")
                  << u8path << U8C(u8" 打开失败") << std::endl;
        return;
    } else {
        std::cout << U8C(u8"u8图片 ")
                  << u8path << U8C(u8" 打开成功!!!!") << std::endl;
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

    ppocr::ocr(out, img);

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
    file::DefFolder     af("./input/app", false);
    list< std::string > p1 = af.get_u8filepath_list( );
    list< std::string > p2 = af.get_u8file_list( );
    list< std::string > p3 = af.get_u8filename_list( );
    for (const auto &l : p1) {
        std::cout << l << std::endl;
    }
    std::cout << std::endl;
    for (const auto &l : p2) {
        std::cout << l << std::endl;
    }
    std::cout << std::endl;
    for (const auto &l : p3) {
        std::cout << l << std::endl;
    }
    std::cout << std::endl;
    list< std::string > p4 = af.get_u8filepath_list(list< std::string >{ ".xlsx" });
    list< std::string > p5 = af.get_u8file_list(list< std::string >{ ".xlsx" });
    list< std::string > p6 = af.get_u8filename_list(list< std::string >{ ".xlsx" });
    for (const auto &l : p4) {
        std::cout << l << std::endl;
    }
    std::cout << std::endl;
    for (const auto &l : p5) {
        std::cout << l << std::endl;
    }
    std::cout << std::endl;
    for (const auto &l : p6) {
        std::cout << l << std::endl;
    }
    af.copy_files_to("./output/sign_for_QingziClass_out/pdf", list< std::string >{ ".xlsx" });
}

// 测试文件夹的检测
void test_for_check_and_create_folder( ) {
    // 测试相对路径和绝对路径
    std::string relativePath       = "./test_folder";     // 相对路径示例
    std::string nestedRelativePath = "./parent/child";    // 嵌套的相对路径（注意：parent文件夹也会被创建）

    // 检测并创建文件夹
    file::create_folder_recursive(relativePath);
    file::create_folder_recursive(nestedRelativePath);
}

// 测试icu_encoding_handler
void test_for_icu_encoding_handler( ) {
    std::string p = ICUEncodingHandler::get_system_default_encoding( );
    std::cout << p << std::endl;

    std::string                            anycode = "./input/att_ims/青宣班1.jpeg";
    std::vector< EncodingDetectionResult > results;    // 结果函数
    std::string                            out;        // 输出
    std::string                            e;          // 错误
    ICUEncodingHandler::detect_encoding(anycode.c_str( ), anycode.size( ), results, 5);

    for (auto &r : results) {
        std::cout << r.encodingName << "    " << r.confidence << std::endl;
    }
    std::cout << encoding::sysdcode_to_utf8(anycode);
}

// 测试排序函数
void test_for_sort_table_by( ) {
    table< std::string > sheet = {
        { U8C(u8"序号"), U8C(u8"姓名"), U8C(u8"学号"), U8C(u8"签到") },
        { "1", U8C(u8"王二"), "20242246", "" },
        { "2", U8C(u8"李四"), "20232435", "" },
        { "3", U8C(u8"张三"), "20224344", "" },
        { "4", U8C(u8"赵六"), "20254934", "" }
    };
    for (const auto &r : sheet) {
        for (const auto &c : r) {
            std::cout << c << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    sort_table_string_by(sheet, 2, false);
    for (const auto &r : sheet) {
        for (const auto &c : r) {
            std::cout << c << "\t";
        }
        std::cout << std::endl;
    }
}

// 测试手动透视矫正类
void test_for_ManualDocPerspectiveCorrector( ) {

    file::DefFolder imgFolder(file::_INPUT_ATT_IMGS_DIR_, false);
    auto            filelist = imgFolder.get_sysfile_list( );
    auto            pathlist = imgFolder.get_sysfilepath_list( );

    for (size_t i = 0; i < pathlist.size( ); i++) {
        cv::Mat us;
        us = cv::imread(pathlist[i]);
        if (us.empty( )) {
            std::cout << encoding::sysdcode_to_utf8(pathlist[i]) << U8C(u8"打开失败") << std::endl;
            return;
        }
        img::ManualDocPerspectiveCorrector c(us, filelist[i]);
        cv::Mat                            out = c.get_corrected_img( );
        cv::imwrite(filelist[i], out);
    }
}
