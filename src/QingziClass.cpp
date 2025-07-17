
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
    int outWhichSheet = 1;    // 生成那一张表：1签到表  2出勤记录表
    while (a != 1 && a != 2) {
        system("cls");
        std::cout << anycode_to_utf8("请选择要生成excel表的类型：") << std::endl
                  << anycode_to_utf8("1. 活动签到表") << std::endl;
        std::cout << anycode_to_utf8("2. 出勤记录表") << std::endl;
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

    /* 4.加载签到表或是出勤记录表 ============================================================= */
    if (outWhichSheet == 1) {
        // 加载报名表
        make_attendanceSheet( );
    } else if (outWhichSheet == 2) {
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
 * @brief 制作签到表
 */
void DoQingziClass::make_attendanceSheet( ) {
    std::vector< std::string >    app_classname;          // 班级名称
    std::vector< std::string >    app_filePathAndName;    // applicationSheet的excel文件的路径
    std::vector< DefUnstdPerson > app_person;             // 定义从报名表中获得的人员信息

    // lambda函数定义========================================================================/
    /*
     * @brief 保存报名表中的信息
     * @param 表格信息
     * @param 班级名称
     */
    auto save_application =
        [&app_person](const std::vector< std::vector< std::string > > &sh, std::string cn) -> void {
        for (size_t rowIndex = 1; rowIndex < sh.size( ); rowIndex++) {
            DefUnstdPerson per;
            per.classname = cn;
            for (size_t colIndex = 0;
                 colIndex < sh[rowIndex].size( ) && sh[rowIndex][colIndex].size( ) != 0;
                 colIndex++) {
                per.information[sh[0][colIndex]] = sh[rowIndex][colIndex];
            }
            app_person.push_back(per);
        }
    };
    //=======================================================================================/

    get_filepath_from_folder(app_classname, app_filePathAndName, anycode_to_utf8("./input/otr/"));
    for (auto it_app_filepath = app_filePathAndName.begin( ), it_app_classname = app_classname.begin( );
         it_app_filepath != app_filePathAndName.end( ) && it_app_classname != app_classname.end( );
         it_app_filepath++, it_app_classname++) {
        // 保存读取到的表格
        std::vector< std::vector< std::string > > sheet;
        load_sheet_from_file(sheet, *it_app_filepath);
        save_application(sheet, *it_app_classname);
    }

    /* 制表 */
    if (perInFormat_ == PersonFormat::STD) {

    } else if (perInFormat_ == PersonFormat::UNSTD) {
        for (auto it = app_person.begin( ); it != app_person.end( ); it++) {
        }
    }
}

/*
 * @brief 搜索，从全人员名单中搜素目标人员信息
 * @param 总名单的一个迭代器
 * @param 目标的人员信息
 * @note 可以考虑怎么优化这四个search函数
 * @shit if很多吧，慢慢看  (^_^)
 */
void DoQingziClass::search_person(
    std::vector< DefStdPerson >::iterator &it_output,
    DefStdPerson                           _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.name == it_all->name) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (_targetPerson.studentID == it_all->studentID) {
                        it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.name == it_all->name) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (_targetPerson.studentID == it_all->studentID) {
                        it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        }
    }
}

/*
 * @brief 搜索，从全人员名单中搜素目标人员信息
 * @param 总名单的一个迭代器
 * @param 目标的人员信息
 * @note 可以考虑怎么优化这四个search函数
 * @shit if很多吧，慢慢看  (^_^)
 */
void DoQingziClass::search_person(
    std::vector< DefStdPerson >::iterator &it_output,
    DefUnstdPerson                         _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.information[anycode_to_utf8("姓名")] == it_all->name) {
                if (_targetPerson.information.find(anycode_to_utf8("学号")) != _targetPerson.information.end( )) {
                    if (_targetPerson.information[anycode_to_utf8("学号")] == it_all->studentID) {
                        it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.information[anycode_to_utf8("姓名")] == it_all->name) {
                if (_targetPerson.information.find(anycode_to_utf8("学号")) != _targetPerson.information.end( )) {
                    if (_targetPerson.information[anycode_to_utf8("学号")] == it_all->studentID) {
                        it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        }
    }
}

/*
 * @brief 搜索，从全人员名单中搜素目标人员信息
 * @param 总名单的一个迭代器
 * @param 目标的人员信息
 * @note 可以考虑怎么优化这四个search函数
 * @shit if很多吧，慢慢看  (^_^)
 */
void DoQingziClass::search_person(
    std::vector< DefUnstdPerson >::iterator &it_output,
    DefStdPerson                             _targetPerson) {
    for (auto it_all = personUnstd_.begin( ); it_all != personUnstd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.name == it_all->information[anycode_to_utf8("姓名")]) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (_targetPerson.studentID == it_all->information[anycode_to_utf8("学号")]) {
                        it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.name == it_all->information[anycode_to_utf8("姓名")]) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (_targetPerson.studentID == it_all->information[anycode_to_utf8("学号")]) {
                        it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        }
    }
}

/*
 * @brief 搜索，从全人员名单中搜素目标人员信息
 * @param 总名单的一个迭代器
 * @param 目标的人员信息
 * @note 可以考虑怎么优化这四个search函数
 * @shit if很多吧，慢慢看  (^_^)
 */
void DoQingziClass::search_person(
    std::vector< DefUnstdPerson >::iterator &it_output,
    DefUnstdPerson                           _targetPerson) {
    for (auto it_all = personUnstd_.begin( ); it_all != personUnstd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.information[anycode_to_utf8("姓名")] == it_all->information[anycode_to_utf8("姓名")]) {
                if (_targetPerson.information.find(anycode_to_utf8("学号")) != _targetPerson.information.end( )) {
                    if (_targetPerson.information[anycode_to_utf8("学号")] == it_all->information[anycode_to_utf8("学号")]) {
                        it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.information[anycode_to_utf8("姓名")] == it_all->information[anycode_to_utf8("姓名")]) {
                if (_targetPerson.information.find(anycode_to_utf8("学号")) != _targetPerson.information.end( )) {
                    if (_targetPerson.information[anycode_to_utf8("学号")] == it_all->information[anycode_to_utf8("学号")]) {
                        it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        }
    }
}
