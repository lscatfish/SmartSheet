
#include "ChineseEncoding.h"
#include "Files.h"
#include "PersonnelInformation.h"
#include "test.h"
#include <algorithm>
#include <consoleapi2.h>
#include <exception>
#include <helper.h>
#include <imgs.h>
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
    get_filepath_from_folder(className_, filePathAndName_, anycode_to_utf8("./input/all/"), std::vector< std::string >{ ".xlsx" });

    std::vector< std::vector< std::string > > test1 = {
        { anycode_to_utf8("序号"), anycode_to_utf8("姓名"), anycode_to_utf8("学号"), anycode_to_utf8("签到") },
        { anycode_to_utf8("1"), anycode_to_utf8("王二"), anycode_to_utf8("20243546545T"), "" },
        { anycode_to_utf8("2"), anycode_to_utf8("张三"), anycode_to_utf8("324352532423"), "" },
        { anycode_to_utf8("3"), anycode_to_utf8("李四"), anycode_to_utf8("324234"), "" },

    };
    save_sheet_to_xlsx(test1, "test1.xlsx", anycode_to_utf8("测试签到表"));
}

// 测试imgs的网格生成模型
bool test_for_grid( ) {
    // 生成0-1080范围的测试点列（模拟网格分布）
    auto generate_test_points = []( ) -> std::vector< GridPoint > {
        std::vector< GridPoint > points;

        // 模拟一个网格：
        // x方向：200, 400, 600, 800（间隔200）
        // y方向：300, 600, 900（间隔300）
        // 每个点添加±5的随机误差，且故意缺失部分点

        // 第一行（y≈300）
        points.emplace_back(202, 298);    // x≈200
        points.emplace_back(401, 303);    // x≈400
        // 故意缺失x=600的点
        points.emplace_back(799, 297);    // x≈800

        // 第二行（y≈600）
        points.emplace_back(203, 602);    // x≈200
        // 故意缺失x=400的点
        points.emplace_back(598, 599);    // x≈600
        points.emplace_back(802, 601);    // x≈800

        // 第三行（y≈900）
        points.emplace_back(199, 903);    // x≈200
        points.emplace_back(402, 898);    // x≈400
        points.emplace_back(601, 901);    // x≈600
        // 故意缺失x=800的点

        return points;
    };

    // 模拟输入：含密集点和稀疏点的网格
    std::vector< GridPoint > points = generate_test_points( );
    for (const auto &p : points) {
        std::cout << "(" << p.first << ", " << p.second << ")\n";
    }
    std::cout << "\n";

    // 区域范围（0-1080）
    double x0 = 1080.0;
    double y0 = 1080.0;

    try {
        // 自动计算阈值（增大比例系数，设置最小阈值为区域的 1%）
        auto [epsilon_x, epsilon_y] = auto_calculate_epsilon(points, x0, y0, 0.5, 0.01);

        std::cout << anycode_to_utf8("自动计算的误差阈值：\n");
        std::cout << "epsilon_x =" << epsilon_x << "\n";
        std::cout << "epsilon_y =" << epsilon_y << "\n\n";
        pause( );

        // 计算最小有效间距（避免生成过密网格线）
        double min_spacing_x = std::max(epsilon_x * 2, 25.0);    // 最小间距
        double min_spacing_y = std::max(epsilon_y * 2, 25.0);

        // 提取网格（传入最小有效间距）
        GridResult grid = extract_grid_info(points, epsilon_x, epsilon_y, min_spacing_x, min_spacing_y);

        // 输出结果
        std::cout << anycode_to_utf8("网格行坐标（y 值）：\n");
        for (double y : grid.row_coords) {
            std::cout << y << " ";
        }
        std::cout << "\n\n";
        pause( );
        std::cout << anycode_to_utf8("网格列坐标（x 值）：\n");
        for (double x : grid.col_coords) {
            std::cout << x << " ";
        }
        std::cout << "\n\n";
        pause( );

        std::cout << anycode_to_utf8("网格交点坐标：\n");
        for (const auto &point : grid.intersections) {
            std::cout << "(" << point.first << "," << point.second << ")\n";
        }
    } catch (const std::exception &e) {
        std::cerr << anycode_to_utf8("错误：") << e.what( ) << std::endl;
        return 1;
    }

    return 0;
}

// 测试load_sheet_from_img
void test_for__load_sheet_from_img( ) {
    std::cout << std::endl
              << std::endl;
    std::vector< std::vector< std::string > > sheet;
    load_sheet_from_img(sheet, "2.jpg");
    for (const auto &row : sheet) {
        for (const auto &cell : row) {
            std::cout << cell << "    ";
        }
        std::cout << std::endl;
    }
}