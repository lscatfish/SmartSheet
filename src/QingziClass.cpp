
#include "ChineseEncoding.h"
#include "Files.h"
#include "QingziClass.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <PersonnelInformation.h>
#include <string>
#include <thread>
#include <vector>
#include <Windows.h>

DoQingziClass::DoQingziClass( ) {
    perInFormat_ = PersonFormat::STD;
}

DoQingziClass::~DoQingziClass( ) {
}

/*
 * @brief 主控函数
 * @note 这个函数写的有一点像屎山，后来者可以考虑重构
 */
void DoQingziClass::start( ) {
    /* 1.选择要载入的人员名单的类型 ======================================================= */
    int a = 0;
    while (a != 1 && a != 2) {
        system("cls");
        std::cout << anycode_to_utf8("请选择名单类型：") << std::endl
                  << anycode_to_utf8("1. 标准名单") << std::endl
                  << anycode_to_utf8(" @标准名单按照以下格式排列：") << std::endl
                  << anycode_to_utf8("  *第一行为  |序号|姓名|性别|年级|学号|政治面貌|学院|专业|联系电话|QQ号|") << std::endl
                  << anycode_to_utf8("  *有效区域应当全部全部填充，若在有效区域内有未填充的单元格，应当全部用符号“-”填充") << std::endl;
        std::cout << anycode_to_utf8("2. 非标准名单") << std::endl
                  << anycode_to_utf8(" @非标准名单按照以下格式排列：") << std::endl
                  << anycode_to_utf8("  *第一行应当为表头") << std::endl
                  << anycode_to_utf8("  *有效区域应当全部全部填充，若在有效区域内有未填充的单元格，应当全部用符号“-”填充") << std::endl;
        std::cout << anycode_to_utf8("请选择（ 输入 1 或者 2 后按下 Enter键 ）：");
        std::cin >> a;
        if (a == 1) {
            perInFormat_ = PersonFormat::STD;
        } else if (a == 2) {
            perInFormat_ = PersonFormat::UNSTD;
        } else {
            std::cout << anycode_to_utf8("你的输入错误，请输入 1 或者 2 后按下 Enter键 ") << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    /* 2.加载全学员表 ===================================================================== */
    load_personnel_information_list( );

    /* 3.选择生成签到表或出勤表 =========================================================== */
    a                 = 0;
    int outWhichSheet = 1;    // 生成那一张表：1签到表  2出勤表
    while (a != 1 && a != 2) {
        system("cls");
        std::cout << anycode_to_utf8("请选择要生成excel表的类型：") << std::endl
                  << anycode_to_utf8("1. 活动签到表") << std::endl;
        std::cout << anycode_to_utf8("2. 出勤表") << std::endl;
        std::cout << anycode_to_utf8("请选择（ 输入 1 或者 2 后按下 Enter键 ）：");
        std::cin >> a;
        if (a == 1) {
            outWhichSheet = 1;
        } else if (a == 2) {
            outWhichSheet = 2;
        } else {
            std::cout << anycode_to_utf8("你的输入错误，请输入 1 或者 2 后按下 Enter键 ") << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    /* 4.加载签到表或是出勤表 ============================================================= */
    if (outWhichSheet == 1) {
        load_applicationSheet_for_attendanceSheet( );
    }
}

/*
 * @brief 加载全学员表的函数
 */
void DoQingziClass::load_personnel_information_list( ) {
    // lambda函数定义========================================================================/
    /*
     * @brief 保存标准的人员信息
     * @param 二维向量用于储存表格信息
     * @param 青字班的名字
     */
    auto save_information_std =
        [&](const std::vector< std::vector< std::string > > &sh, std::string cn) -> void {
        for (size_t rowIndex = 1; rowIndex < sh.size( ); rowIndex++) {
            DefStdPerson per;
            per.classname        = cn;
            per.name             = sh[rowIndex][1];
            per.gender           = sh[rowIndex][2];
            per.grade            = sh[rowIndex][3];
            per.studentID        = sh[rowIndex][4];
            per.politicaloutlook = sh[rowIndex][5];
            per.academy          = sh[rowIndex][6];
            per.majors           = sh[rowIndex][7];
            per.phonenumber      = sh[rowIndex][8];
            per.qqnumber         = sh[rowIndex][9];
            personStd_.push_back(per);
        }
    };
    /*
     * @brief 保存非标准的人员信息
     * @param 二维向量用于储存表格信息
     * @param 青字班的名字
     */
    auto save_information_unstd =
        [&](const std::vector< std::vector< std::string > > &sh, std::string cn) -> void {
        for (size_t rowIndex = 1; rowIndex < sh.size( ); rowIndex++) {
            DefUnstdPerson per;
            per.classname = cn;
            for (size_t colIndex = 0;
                 colIndex < sh[rowIndex].size( ) && sh[rowIndex][colIndex].size( ) != 0;
                 colIndex++) {
                per.information[sh[0][colIndex]] = sh[rowIndex][colIndex];
            }
            personUnstd_.push_back(per);
        }
    };
    //=======================================================================================/

    get_filepath_from_folder(className_, filePathAndName_, anycode_to_utf8("./input/all/"));
    // 按文件读取每个青字班的信息表
    for (auto it_className = className_.begin( ), it_filePathAndName = filePathAndName_.begin( );
         it_className != className_.end( ) && it_filePathAndName != filePathAndName_.end( );
         it_className++, it_filePathAndName++) {
        // 保存读取到的表格
        std::vector< std::vector< std::string > > sheet;
        load_sheet_from_file(sheet, *it_filePathAndName);
        if (perInFormat_ == PersonFormat::STD) {
            save_information_std(sheet, *it_className);
        } else if (perInFormat_ == PersonFormat::UNSTD) {
            save_information_unstd(sheet, *it_className);
        }
    }
}

/*
 * @brief 加载签到表（报名表）
 */
void DoQingziClass::load_applicationSheet_for_attendanceSheet( ) {


}
