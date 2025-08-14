
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <Encoding.h>
#include <Files.h>
#include <Fuzzy.h>
#include <helper.h>
#include <iostream>
#include <map>
#include <PersonnelInformation.h>
#include <ppocr_API.h>
#include <QingziClass.h>
#include <string>
#include <test.h>
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
}

DoQingziClass::~DoQingziClass( ) {
}

/*
 * @brief 主控函数
 * @note 这个函数写的有一点像屎山，后来者可以考虑重构
 * @attention 重构到构造函数吧
 */
void DoQingziClass::start( ) {
    int a = 0;
    /* 1.加载全学员表 ===================================================================== */
    system("cls");
    std::cout << u8"加载全学员信息表..." << std::endl;
    load_personnel_information_list( );

    /* 3.选择生成签到表或出勤表 =========================================================== */
    a                 = 0;
    int outWhichSheet = 1;    // 生成那一张表：1签到表  2出勤记录表
    while (a != 1 && a != 2) {
        system("cls");
        std::cout << u8"请选择要生成excel表的类型：" << std::endl
                  << u8"1. 活动签到表" << std::endl;
        std::cout << u8"2. 出勤记录表" << std::endl;
        std::cout << u8"请选择（ 输入 1 或者 2 后按下 Enter键 ）：";
        std::cin >> a;
        if (a == 1) {
            outWhichSheet = 1;
        } else if (a == 2) {
            outWhichSheet = 2;
        } else {
            std::cout << u8"你的输入错误，请输入 1 或者 2 后按下 Enter键 " << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    /* 4.加载签到表或是出勤记录表 ============================================================= */
    if (outWhichSheet == 1) {
        // 制作签到表
        make_attendanceSheet( );
        save_attendanceSheet( );
        if (unknownPerson_.size( ) >= 1) {
            std::cout << std::endl
                      << std::endl
                      << "\033[43;30mWARNING!!!\033[0m" << std::endl;
            std::cout << "\033[43;30m";
            std::cout << "###ATTENTION### ";
            std::cout << u8"以下的人员不在全学员名单中";
            std::cout << " ###ATTENTION###";
            std::cout << std::endl;
            for (auto it_unknownPerson = unknownPerson_.begin( );
                 it_unknownPerson != unknownPerson_.end( );
                 it_unknownPerson++) {
                if (it_unknownPerson->personStd.ifcheck == false) {
                    std::cout << "Unknown:  ";
                    std::cout << it_unknownPerson->personStd.classname << "    ";
                    std::cout << it_unknownPerson->personStd.name << "    ";
                    if (it_unknownPerson->personStd.studentID.size( ) != 0) {
                        std::cout << it_unknownPerson->personStd.studentID << "    ";
                    } else {
                        std::cout << u8"？学号不存在？ ";
                    }
                    std::cout << std::endl;
                    if (it_unknownPerson->likelyPerson.size( ) != 0) {
                        for (const auto &likelyPer : it_unknownPerson->likelyPerson) {
                            std::cout << u8"-likely:  ";
                            std::cout << likelyPer.classname << "    ";
                            std::cout << likelyPer.name << "    ";
                            std::cout << likelyPer.studentID << "    ";
                            std::cout << std::endl;
                        }
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << "###ATTENTION### ";
            std::cout << u8"以上的人员不在全学员名单中";
            std::cout << " ###ATTENTION###";
            std::cout << "\033[0m";
            std::cout << std::endl
                      << std::endl
                      << std::endl;
        }
        std::cout << u8"已完成签到表输出，请在 output/app_out 中查看！" << std::endl
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return;
    } else if (outWhichSheet == 2) {
        // 制作考勤统计表
        std::cout << std::endl
                  << u8"此功能还在开发中..." << std::endl;
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
#ifdef DO_TEST
        for (size_t rowIndex = 0; rowIndex < sh.size( ); rowIndex++) {
            for (size_t colIndex = 0;
                 colIndex < sh[rowIndex].size( ) && sh[rowIndex][colIndex].size( ) != 0;
                 colIndex++) {
                std::cout << sh[rowIndex][colIndex] << "  ";
            }
            std::cout << std::endl;
        }
#endif    // DO_TEST

        // 这里实际上应该先转成defline，在转成defperson
        for (size_t rowIndex = 1; rowIndex < sh.size( ); rowIndex++) {
            DefPerson per;
            per.classname = cn;
            //  std::cout << chcode_to_utf8("加载std") << std::endl;
            for (size_t colIndex = 0;
                 colIndex < sh[rowIndex].size( ) && sh[rowIndex][colIndex].size( ) != 0;
                 colIndex++) {
                if (sh[0][colIndex] == u8"姓名") {
                    per.name = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == u8"性别") {
                    per.gender = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == u8"年级") {
                    per.grade = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == u8"学号") {
                    per.studentID = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == u8"政治面貌") {
                    per.politicaloutlook = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == u8"学院") {
                    per.academy = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == u8"专业") {
                    per.majors = sh[rowIndex][colIndex];
                } else if ((sh[0][colIndex] == u8"电话")
                           || (sh[0][colIndex] == u8"联系方式")
                           || (sh[0][colIndex] == u8"联系电话")
                           || (sh[0][colIndex] == u8"电话号码")) {
                    per.phonenumber = sh[rowIndex][colIndex];
                } else if ((sh[0][colIndex] == u8"QQ号")
                           || (sh[0][colIndex] == u8"qq号")
                           || (sh[0][colIndex] == u8"qq")
                           || (sh[0][colIndex] == u8"QQ")) {
                    per.qqnumber = sh[rowIndex][colIndex];
                } else {
                    per.otherInformation[sh[0][colIndex]] = sh[rowIndex][colIndex];
#ifdef DO_TEST
                    std::cout << "row" << rowIndex << ": " << sh[0][colIndex] << "  " << sh[rowIndex][colIndex] << std::endl;
#endif
                }
            }
            personStd_.push_back(per);
        }
    };
    //=======================================================================================/

    std::cout << std::endl
              << u8"读取全学院名单..." << std::endl;
    get_filepath_from_folder(
        className_,
        filePathAndName_,
        "./input/all/",
        std::vector< std::string >{ ".xlsx" });
    std::cout << std::endl;
    // 按文件读取每个青字班的信息表
    for (auto it_className = className_.begin( ), it_filePathAndName = filePathAndName_.begin( );
         it_className != className_.end( ) && it_filePathAndName != filePathAndName_.end( );
         it_className++, it_filePathAndName++) {
        // 保存读取到的表格
        std::vector< std::vector< std::string > > sheet;
        load_sheet_from_xlsx(sheet, *it_filePathAndName);
        save_information_std(sheet, *it_className);
    }
}

/*
 * @brief 制作签到表
 */
void DoQingziClass::make_attendanceSheet( ) {
    std::vector< std::string > app_classname;          // 班级名称
    std::vector< std::string > app_filePathAndName;    // applicationSheet的excel文件的路径
    std::vector< DefLine >     app_person;             // 定义从报名表中获得的人员信息

    // lambda函数定义========================================================================/
    /*
     * @brief 保存报名表中的信息
     * @param 表格信息
     * @param 班级名称
     */
    auto extract_application_to_vector =
        [&app_person](const std::vector< std::vector< std::string > > &sh, std::string cn) -> void {
        for (size_t rowIndex = 1; rowIndex < sh.size( ); rowIndex++) {
            DefLine per;
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
              << u8"读取各班的报名表..." << std::endl;
    get_filepath_from_folder(
        app_classname,
        app_filePathAndName,
        "./input/app/",
        std::vector< std::string >{ ".xlsx" });
    std::cout << std::endl;

    for (auto it_app_filepath = app_filePathAndName.begin( ), it_app_classname = app_classname.begin( );
         it_app_filepath != app_filePathAndName.end( ) && it_app_classname != app_classname.end( );
         it_app_filepath++, it_app_classname++) {
        // 保存读取到的表格
        std::vector< std::vector< std::string > > sheet;
        load_sheet_from_xlsx(sheet, *it_app_filepath);
        extract_application_to_vector(sheet, *it_app_classname);
    }

    /* 制表 */
    for (auto it_app_person = app_person.begin( ); it_app_person != app_person.end( ); it_app_person++) {
        std::vector< DefPerson >::iterator it_search = personStd_.end( );    // 赋值到哨兵迭代器
        search_person(it_search, *it_app_person);
        if (it_search != personStd_.end( )) {    // 说明搜索到了
            it_search->ifsign      = true;
            it_app_person->ifcheck = true;    // 这里的ifcheck说明报了名的人已经匹配
        } else {
            it_app_person->ifcheck = false;    // 没有搜索到
        }
    }

    // 标定没有搜索到的人
    for (auto it_app_person = app_person.begin( );
         it_app_person != app_person.end( );
         it_app_person++) {
        if (it_app_person->ifcheck == false) {
            if (it_app_person->information[u8"姓名"].size( ) != 0) {
                DefUnknownPerson unP;
                unP.personLine = *it_app_person;
                trans_line_to_person(unP.personLine, unP.personStd);
                unknownPerson_.push_back(unP);
            }
        }
    }
    // 模糊匹配没有搜到的人
    for (auto &unknownPerson : unknownPerson_) {
        fuzzy::search_for_person(unknownPerson.likelyPerson, unknownPerson.personStd, personStd_);
    }
}

/*
 * @brief 保存签到表
 */
void DoQingziClass::save_attendanceSheet( ) {
    for (auto it_classname = className_.begin( ); it_classname != className_.end( ); it_classname++) {
        std::string                               sheetTitle = *it_classname + u8"签到表";
        std::string                               sheetPath  = "./output/app_out/" + (*it_classname) + ".xlsx";
        std::vector< std::vector< std::string > > sheet      = {
            { u8"序号", u8"姓名", u8"学号", u8"签到" }
        };
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
        save_sheet_to_xlsx(sheet, sheetPath, sheetTitle);
    }
}

/*
 * @brief 制作考勤统计表
 */
void DoQingziClass::make_statisticsSheet( ) {

    /*  ===================================================================
     *				================
     *				||函数工作说明||
     *				================
     * @brief 路径"./input/att/"中拉取图片的路径与名称
     * 图片的命名规则：青x班1   青x班2    (末尾的数字是某班级签到表照片的第i张)
     *
     * ----------------------------------------------------------------- */


    std::map< std::string, std::vector< std::string > > classname__filePathAndName;    // 班级名称与各班的签到表的匹配
    std::vector< std::string >                          att_fileName;                  // 图片的文件名(无后缀)
    std::vector< std::string >                          att_filePathAndName;           // 图片-签到表文件的路径
    std::vector< DefLine >                              att_person;                    // 定义从签到表中获得的人员信息

    // lambda函数定义========================================================================/
    /*
     * @brief 提取签到表中的信息
     * @param 表格信息
     * @param 班级名称
     */
    auto extract_attendance_to_vector =
        [&att_person](const std::vector< std::vector< std::string > > &sh, std::string cn) -> void {
        for (size_t rowIndex = 1; rowIndex < sh.size( ); rowIndex++) {
            DefLine per;
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

    // 1.拉取文件夹中的所有照片的地址
    get_filepath_from_folder(
        att_fileName,
        att_filePathAndName,
        u8"./input/att/",
        std::vector< std::string >{ ".jpg", ".png", ".jpeg", ".tiff", ".tif ",
                                    ".jpe", ".bmp", ".dib", ".webp", ".raw" });
    // 排序，这样就可以按照班级来
    std::sort(att_fileName.begin( ), att_fileName.end( ));
    std::sort(att_filePathAndName.begin( ), att_filePathAndName.end( ));

    // 2.解析班级的名字与名单的数量，储存到classname__filePathAndName中
    for (auto it_att_fileName = att_fileName.begin( ), it_att_filePathAndName = att_filePathAndName.begin( );
         it_att_fileName != att_fileName.end( ) && it_att_filePathAndName != att_filePathAndName.end( );
         it_att_fileName++, it_att_filePathAndName++) {
        auto [chinese, number] = split_chinese_and_number(*it_att_fileName);
        classname__filePathAndName[chinese].push_back(*it_att_filePathAndName);
    }

    // 3.

    // for (auto it_att_filepath = att_filePathAndName.begin( ), it_att_classname = att_classname.begin( );
    //      it_att_filepath != att_filePathAndName.end( ) && it_att_classname != att_classname.end( );
    //      it_att_filepath++, it_att_classname++) {
    //     // 保存读取到的表格
    //     std::vector< std::vector< std::string > > sheet;
    //     load_sheet_from_xlsx(sheet, *it_att_filepath);
    //     extract_attendance_to_vector(sheet, *it_att_classname);
    // }




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
 * @shit if很多吧，慢慢看  (^-^)
 */
void DoQingziClass::search_person(
    std::vector< DefPerson >::iterator &_it_output,
    DefPerson                           _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.name == it_all->name) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (compare_studentID(_targetPerson.studentID, it_all->studentID)) {
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
                    if (compare_studentID(_targetPerson.studentID, it_all->studentID)) {
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
 * @shit if很多吧，慢慢看  (^-^)
 */
void DoQingziClass::search_person(
    std::vector< DefPerson >::iterator &_it_output,
    DefLine                             _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.information[u8"姓名"] == it_all->name) {
                if (_targetPerson.information.find(u8"学号") != _targetPerson.information.end( )) {
                    if (compare_studentID(_targetPerson.information[u8"学号"], it_all->studentID)) {
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
            if (_targetPerson.information[u8"姓名"] == it_all->name) {
                if (_targetPerson.information.find(u8"学号") != _targetPerson.information.end( )) {
                    if (compare_studentID(_targetPerson.information[u8"学号"], it_all->studentID)) {
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
 * @brief 比较学号
 * @return 相同返回true  不同返回false
 */
bool DoQingziClass::compare_studentID(const std::string &a, const std::string &b) {
    // 关键是最后一位的T的大小写
    if (a.size( ) == b.size( )) {
        auto ita_cr = a.crbegin( );
        auto itb_cr = b.crbegin( );
        if ((*ita_cr == 't' || *ita_cr == 'T') && (*itb_cr == 't' || *itb_cr == 'T')) {
            for (int index = 0; index < a.size( ) - 1; index++) {
                if (a[index] != b[index]) {
                    return false;
                }
            }
            return true;
        } else {
            if (a == b)
                return true;
            else
                return false;
        }
    } else
        return false;
}

/*
 * @brief 一行信息转化为标准人员信息
 * @param 一行信息
 * @param 标准的人员信息
 */
void DoQingziClass::trans_line_to_person(const DefLine &_inperLine, DefPerson &_outperStd) {
    DefPerson per;
    per.classname = _inperLine.classname;
    per.ifcheck   = _inperLine.ifcheck;
    per.ifsign    = _inperLine.ifcheck;
    for (auto it_inperLine = _inperLine.information.begin( );
         it_inperLine != _inperLine.information.end( );
         it_inperLine++) {
        if (it_inperLine->first == u8"姓名") {
            per.name = it_inperLine->second;
        } else if (it_inperLine->first == u8"性别") {
            per.gender = it_inperLine->second;
        } else if (it_inperLine->first == u8"年级") {
            per.grade = it_inperLine->second;
        } else if (it_inperLine->first == u8"学号") {
            per.studentID = it_inperLine->second;
        } else if (it_inperLine->first == u8"政治面貌") {
            per.politicaloutlook = it_inperLine->second;
        } else if (it_inperLine->first == u8"学院") {
            per.academy = it_inperLine->second;
        } else if (it_inperLine->first == u8"专业") {
            per.majors = it_inperLine->second;
        } else if ((it_inperLine->first == u8"电话")
                   || (it_inperLine->first == u8"联系方式")
                   || (it_inperLine->first == u8"联系电话")
                   || (it_inperLine->first == u8"电话号码")) {
            per.phonenumber = it_inperLine->second;
        } else if ((it_inperLine->first == u8"QQ号")
                   || (it_inperLine->first == u8"qq号")
                   || (it_inperLine->first == u8"qq")
                   || (it_inperLine->first == u8"QQ")) {
            per.qqnumber = it_inperLine->second;
        } else {
            per.otherInformation[it_inperLine->first] = it_inperLine->second;
        }
    }
    _outperStd = per;
}

/*
 * @brief 标准人员信息转化为一行信息
 * @param 标准的人员信息
 * @param 一行信息
 */
void DoQingziClass::trans_person_to_line(const DefPerson &_inperStd, DefLine _outperLine) {
    using namespace encoding;
    DefLine per;
    per.classname                               = _inperStd.classname;
    per.information[chcode_to_utf8("姓名")]     = _inperStd.name;
    per.information[chcode_to_utf8("性别")]     = _inperStd.gender;
    per.information[chcode_to_utf8("年级")]     = _inperStd.grade;
    per.information[chcode_to_utf8("学号")]     = _inperStd.studentID;
    per.information[chcode_to_utf8("政治面貌")] = _inperStd.politicaloutlook;
    per.information[chcode_to_utf8("学院")]     = _inperStd.academy;
    per.information[chcode_to_utf8("专业")]     = _inperStd.majors;
    per.information[chcode_to_utf8("QQ号")]     = _inperStd.qqnumber;
    per.ifcheck                                 = _inperStd.ifcheck;
    per.ifsign                                  = _inperStd.ifcheck;
    if (_inperStd.otherInformation.size( ) != 0) {
        for (auto it_in = _inperStd.otherInformation.begin( );
             it_in != _inperStd.otherInformation.end( );
             it_in++) {
            per.information[it_in->first] = it_in->second;
        }
    }
    _outperLine = per;
}
