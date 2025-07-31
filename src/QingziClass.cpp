
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

/*
 * @brief 将整数类型转化为string类型
 * @param 输入的整型
 */
static std::string trans_integer_to_string(int in) {
    return std::to_string(in);
}

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
        // 制作签到表
        make_attendanceSheet( );
        save_attendanceSheet( );
        if (errorPerson_.size( ) >= 1) {
            std::cout << std::endl
                      << std::endl
                      << "\033[43;30mWARNING!!!\033[0m" << std::endl;
            std::cout << "\033[43;30m";
            std::cout << "###ATTENTION### ";
            std::cout << anycode_to_utf8("以下的人员不在全学员名单中");
            std::cout << " ###ATTENTION###";
            std::cout << std::endl;
            for (auto it_errorPerson = errorPerson_.begin( ); it_errorPerson != errorPerson_.end( ); it_errorPerson++) {
                if (it_errorPerson->ifcheck == false) {
                    std::cout << it_errorPerson->classname << "    ";
                    std::cout << it_errorPerson->information[anycode_to_utf8("姓名")] << "    ";
                    if (it_errorPerson->information.find(anycode_to_utf8("学号")) != it_errorPerson->information.end( )) {
                        std::cout << it_errorPerson->information[anycode_to_utf8("学号")] << "    ";
                    } else {
                        std::cout << anycode_to_utf8("？学号不存在？") << " ";
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << "###ATTENTION### ";
            std::cout << anycode_to_utf8("以上的人员不在全学员名单中");
            std::cout << " ###ATTENTION###";
            std::cout << "\033[0m";
            std::cout << std::endl
                      << std::endl
                      << std::endl;
        }
        std::cout << anycode_to_utf8("已完成签到表输出，请在 output/app_out 中查看！") << std::endl
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return;
    } else if (outWhichSheet == 2) {
        // 制作考勤统计表
        std::cout << std::endl
                  << anycode_to_utf8("此功能还在开发中...") << std::endl;
        return;
        make_attendanceSheet( );
        make_statisticsSheet( );
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
    std::cout << std::endl
              << anycode_to_utf8("读取全学院名单...") << std::endl;
    get_filepath_from_folder(className_, filePathAndName_, anycode_to_utf8("./input/all/"));
    std::cout << std::endl;
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
    auto save_application_to_vector =
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
    std::cout << std ::endl
              << anycode_to_utf8("读取各班的报名表...") << std::endl;
    get_filepath_from_folder(app_classname, app_filePathAndName, anycode_to_utf8("./input/app/"));
    std::cout << std::endl;

    for (auto it_app_filepath = app_filePathAndName.begin( ), it_app_classname = app_classname.begin( );
         it_app_filepath != app_filePathAndName.end( ) && it_app_classname != app_classname.end( );
         it_app_filepath++, it_app_classname++) {
        // 保存读取到的表格
        std::vector< std::vector< std::string > > sheet;
#if false
        std::cout << *it_app_filepath << std::endl;
#endif
        load_sheet_from_file(sheet, *it_app_filepath);
        save_application_to_vector(sheet, *it_app_classname);
    }

    /* 制表 */
    if (perInFormat_ == PersonFormat::STD) {
        for (auto it_app_person = app_person.begin( ); it_app_person != app_person.end( ); it_app_person++) {
            std::vector< DefStdPerson >::iterator it_search = personStd_.end( );    // 赋值到哨兵迭代器
            search_person(it_search, *it_app_person);
            if (it_search != personStd_.end( )) {    // 说明搜索到了
                it_search->ifsign      = true;
                it_app_person->ifcheck = true;    // 这里的ifcheck说明报了名的人已经匹配
            } else {
                it_app_person->ifcheck = false;    // 没有搜索到
            }
        }
    } else if (perInFormat_ == PersonFormat::UNSTD) {
        for (auto it_app_person = app_person.begin( ); it_app_person != app_person.end( ); it_app_person++) {
            std::vector< DefUnstdPerson >::iterator it_search = personUnstd_.end( );    // 赋值到哨兵迭代器
            search_person(it_search, *it_app_person);
            if (it_search != personUnstd_.end( )) {    // 说明搜索到了
                it_search->ifsign      = true;
                it_app_person->ifcheck = true;    // 这里的ifcheck说明报了名的人已经匹配
            } else {
                it_app_person->ifcheck = false;    // 没有搜索到
            }
        }
    }
    // 标定没有搜索到的人
    for (auto it_app_person = app_person.begin( ); it_app_person != app_person.end( ); it_app_person++) {
        if (it_app_person->ifcheck == false) {
            if (it_app_person->information[anycode_to_utf8("姓名")].size( ) != 0)
                errorPerson_.push_back(*it_app_person);
        }
    }
}

/*
 * @brief 保存签到表
 */
void DoQingziClass::save_attendanceSheet( ) {
    for (auto it_classname = className_.begin( ); it_classname != className_.end( ); it_classname++) {
        std::string                               sheetTitle = *it_classname + anycode_to_utf8("签到表");
        std::string                               sheetPath  = "./output/app_out/" + (*it_classname) + ".xlsx";
        std::vector< std::vector< std::string > > sheet      = {
            { anycode_to_utf8("序号"), anycode_to_utf8("姓名"), anycode_to_utf8("学号"), anycode_to_utf8("签到") }
        };

        if (perInFormat_ == PersonFormat::STD) {
            int serialNum = 1;
            for (auto it_person = personStd_.begin( ); it_person != personStd_.end( ); it_person++) {
                if (it_person->classname == *it_classname && it_person->ifsign == true) {
                    std::vector< std::string > aRow;
                    aRow.push_back(trans_integer_to_string(serialNum));
                    aRow.push_back(it_person->name);
                    aRow.push_back(it_person->studentID);
                    aRow.push_back("");
                    sheet.push_back(aRow);
                    serialNum++;
                }
            }
        } else if (perInFormat_ == PersonFormat::UNSTD) {
            int serialNum = 1;
            for (auto it_person = personUnstd_.begin( ); it_person != personUnstd_.end( ); it_person++) {
                if (it_person->classname == *it_classname && it_person->ifsign == true) {
                    std::vector< std::string > aRow;
                    aRow.push_back(trans_integer_to_string(serialNum));
                    aRow.push_back(it_person->information[anycode_to_utf8("姓名")]);
                    aRow.push_back(it_person->information[anycode_to_utf8("学号")]);
                    aRow.push_back("");
                    sheet.push_back(aRow);
                    serialNum++;
                }
            }
        }
        save_sheet_to_file(sheet, sheetPath, sheetTitle);
    }
}

/*
 * @brief 制作考勤统计表
 */
void DoQingziClass::make_statisticsSheet( ) {
    std::vector< std::string >    att_classname;          // 班级名称
    std::vector< std::string >    att_filePathAndName;    // 签到表的excel文件的路径
    std::vector< DefUnstdPerson > att_person;             // 定义从签到表中获得的人员信息

    // lambda函数定义========================================================================/
    /*
     * @brief 保存签到表中的信息
     * @param 表格信息
     * @param 班级名称
     */
    auto save_application =
        [&att_person](const std::vector< std::vector< std::string > > &sh, std::string cn) -> void {
        for (size_t rowIndex = 1; rowIndex < sh.size( ); rowIndex++) {
            DefUnstdPerson per;
            per.classname = cn;
            for (size_t colIndex = 0;
                 colIndex < sh[rowIndex].size( ) && sh[rowIndex][colIndex].size( ) != 0;
                 colIndex++) {
                per.information[sh[0][colIndex]] = sh[rowIndex][colIndex];
            }
            att_person.push_back(per);
        }
    };
    //=======================================================================================/

    get_filepath_from_folder(att_classname, att_filePathAndName, anycode_to_utf8("./input/att/"));
    for (auto it_att_filepath = att_filePathAndName.begin( ), it_att_classname = att_classname.begin( );
         it_att_filepath != att_filePathAndName.end( ) && it_att_classname != att_classname.end( );
         it_att_filepath++, it_att_classname++) {
        // 保存读取到的表格
        std::vector< std::vector< std::string > > sheet;
        load_sheet_from_file(sheet, *it_att_filepath);
        save_application(sheet, *it_att_classname);
    }

    /* 制表 */
    /* 待制作 */
    /* 待制作 */
    /* 待制作 */
    /* 待制作 */
    /* 待制作 */
    /* 待制作 */
    /* 待制作 */
    /* 待制作 */
    /* 待制作 */
}

/*
 * @brief 搜索，从全人员名单中搜素目标人员信息
 * @param 总名单的一个迭代器
 * @param 目标的人员信息
 * @note 可以考虑怎么优化这四个search函数
 * @shit if很多吧，慢慢看  (^_^)
 */
void DoQingziClass::search_person(
    std::vector< DefStdPerson >::iterator &_it_output,
    DefStdPerson                           _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.name == it_all->name) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (_targetPerson.studentID == it_all->studentID) {
                        _it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.name == it_all->name) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (_targetPerson.studentID == it_all->studentID) {
                        _it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
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
    std::vector< DefStdPerson >::iterator &_it_output,
    DefUnstdPerson                         _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.information[anycode_to_utf8("姓名")] == it_all->name) {
                if (_targetPerson.information.find(anycode_to_utf8("学号")) != _targetPerson.information.end( )) {
                    if (_targetPerson.information[anycode_to_utf8("学号")] == it_all->studentID) {
                        _it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.information[anycode_to_utf8("姓名")] == it_all->name) {
                if (_targetPerson.information.find(anycode_to_utf8("学号")) != _targetPerson.information.end( )) {
                    if (_targetPerson.information[anycode_to_utf8("学号")] == it_all->studentID) {
                        _it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
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
    std::vector< DefUnstdPerson >::iterator &_it_output,
    DefStdPerson                             _targetPerson) {
    for (auto it_all = personUnstd_.begin( ); it_all != personUnstd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.name == it_all->information[anycode_to_utf8("姓名")]) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (_targetPerson.studentID == it_all->information[anycode_to_utf8("学号")]) {
                        _it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.name == it_all->information[anycode_to_utf8("姓名")]) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (_targetPerson.studentID == it_all->information[anycode_to_utf8("学号")]) {
                        _it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
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
    std::vector< DefUnstdPerson >::iterator &_it_output,
    DefUnstdPerson                           _targetPerson) {
    for (auto it_all = personUnstd_.begin( ); it_all != personUnstd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.information[anycode_to_utf8("姓名")] == it_all->information[anycode_to_utf8("姓名")]) {
                if (_targetPerson.information.find(anycode_to_utf8("学号")) != _targetPerson.information.end( )) {
                    if (_targetPerson.information[anycode_to_utf8("学号")] == it_all->information[anycode_to_utf8("学号")]) {
                        _it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.information[anycode_to_utf8("姓名")] == it_all->information[anycode_to_utf8("姓名")]) {
                if (_targetPerson.information.find(anycode_to_utf8("学号")) != _targetPerson.information.end( )) {
                    if (_targetPerson.information[anycode_to_utf8("学号")] == it_all->information[anycode_to_utf8("学号")]) {
                        _it_output = it_all;
                        return;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return;
                }
            } else {
                continue;
            }
        }
    }
}
