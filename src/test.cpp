
#include "ChineseEncoding.h"
#include "Files.h"
#include "PersonnelInformation.h"
#include "test.h"
#include <consoleapi2.h>
#include <iostream>
#include <string>
#include <stringapiset.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <xlnt/xlnt.hpp>

void test_main( ) {


    SetConsoleOutputCP(65001);    // 输出代码页设为 UTF-8
    SetConsoleCP(65001);          // 输入代码页也设为 UTF-8

    /* 1. 载入 Excel 文件 ---------------------------------------------------- */
    xlnt::workbook wb;                         // 创建一个工作簿对象
    wb.load(anycode_to_utf8("123我.xlsx"));    // 将磁盘上的 1.xlsx 加载到内存
    auto ws = wb.active_sheet( );              // 获取当前激活的工作表（第一张）

    /* 2. 在控制台提示用户 --------------------------------------------------- */
    std::cout << "正在处理电子表格..." << std::endl;
    std::cout << "正在创建一个总向量，用于存储整个表格内容..." << std::endl;

    /* 3. 准备二维向量保存整张表 -------------------------------------------- */
    // theWholeSpreadSheet[row][col] 即第 row 行第 col 列的字符串
    std::vector< std::vector< std::string > > theWholeSpreadSheet;

    /* 4. 按行遍历工作表 ----------------------------------------------------- */
    // ws.rows(false) 返回行迭代器，false 表示不缓存，节省内存
    for (auto row : ws.rows(false)) {
        std::cout << "为当前行创建一个新的向量..." << std::endl;

        // 保存当前行所有单元格文本的临时向量
        std::vector< std::string > aSingleRow;

        /* 5. 遍历当前行的每个单元格 ---------------------------------------- */
        for (auto cell : row) {
            std::cout << "将该单元格添加到当前行向量..." << std::endl;
            // cell.to_string() 把数字、日期、公式等统一转为字符串
            aSingleRow.push_back(anycode_to_utf8(cell.to_string( )));
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

    wss.active_sheet( ).cell("B6").value(anycode_to_utf8("1中国234"));
    wss.save(anycode_to_utf8("./1我/ou操.xlsx"));


    std::vector< std::string > className_;          // 班级名字
    std::vector< std::string > filePathAndName_;    // 每个xlsx文件的位置
    get_filepath_from_folder(className_, filePathAndName_, anycode_to_utf8("./input/all/"));

    std::vector< std::vector< std::string > > test1 = {
        { anycode_to_utf8("序号"), anycode_to_utf8("姓名"), anycode_to_utf8("学号"), anycode_to_utf8("签到") },
        { anycode_to_utf8("1"), anycode_to_utf8("王二"), anycode_to_utf8("20243546545T"), "" },
        { anycode_to_utf8("2"), anycode_to_utf8("张三"), anycode_to_utf8("324352532423"), "" },
        { anycode_to_utf8("3"), anycode_to_utf8("李四"), anycode_to_utf8("324234"), "" },

    };
    save_sheet_to_file(test1, "test1.xlsx", anycode_to_utf8("测试签到表"));
}
