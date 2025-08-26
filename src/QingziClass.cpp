
#include <algorithm>
#include <basic.hpp>
#include <chrono>
#include <cstdlib>
#include <Encoding.h>
#include <Files.h>
#include <Fuzzy.h>
#include <helper.h>
#include <imgs.h>
#include <iostream>
#include <map>
#include <pdf.h>
#include <PersonnelInformation.h>
#include <ppocr_API.h>
#include <QingziClass.h>
#include <string>
#include <thread>
#include <vector>
#include <word.h>


DoQingziClass::DoQingziClass( ) {
    if (!self_check( ))
        return;
}

DoQingziClass::~DoQingziClass( ) {
}

/*
 * @brief 主控函数
 * @note 这个函数写的有一点像屎山，后来者可以考虑重构
 * @todo 重构到构造函数吧
 */
void DoQingziClass::start( ) {

    /* 3.选择生成签到表或出勤表 =========================================================== */
    int outWhichSheet = choose_function( );    // 生成那一张表：1签到表  2出勤记录表

    /* 4.加载签到表或是出勤记录表 ============================================================= */
    if (outWhichSheet == 1) {
        attendance( );
    } else if (outWhichSheet == 2) {
        statistics( );
    } else if (outWhichSheet == 3) {
        registration( );
    }
}

// 自检程序
bool DoQingziClass::self_check( ) {
    clearConsole( );
    std::cout << U8C(u8"启动自检程序......") << std::endl
              << std::endl;
    std::cout << U8C(u8"检测工作区的文件夹：") << std::endl;
    const list< std::string > ws_pathList{
        "./models/",
        file::_INPUT_ALL_DIR_,
        file::_INPUT_APP_DIR_,
        file::_INPUT_ATT_IMGS_DIR_,
        file::_INPUT_SIGN_QC_ALL_DIR_,
        file::_OUTPUT_APP_DIR_,
        file::_OUTPUT_ATT_DIR_,
        file::_OUTPUT_SIGN_QC_DIR_,
        file::_OUTPUT_SIGN_QC_PDF_DIR_,
        file::_STORAGE_DIR_
    };
    for (const auto &p : ws_pathList) {
        if (!file::create_folder_recursive(p)) {
            pause( );
            return false;
        };
    }

    std::cout << std::endl
              << U8C(u8"检测模型文件：") << std::endl;
    const list< std::string > md_pathList{
        ppocr::_ppocrDir_.cls_model_dir,
        ppocr::_ppocrDir_.det_model_dir,
        ppocr::_ppocrDir_.rec_model_dir
    };
    for (const auto &p : md_pathList) {
        if (file::is_folder_empty(p)) {
            std::cout << U8C(u8"模型文件夹错误：") << p << std::endl;
            pause( );
            return false;
        }
    }
    if (!file::is_file_exists(ppocr::_ppocrDir_.rec_char_dict_path)) {
        std::cout << U8C(u8"模型字典库错误：")
                  << ppocr::_ppocrDir_.rec_char_dict_path << std::endl;
        pause( );
        return false;
    }
    std::cout << U8C(u8"模型文件检测通过！！！") << std::endl;
    std::cout << std::endl
              << U8C(u8"自检完毕...") << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    pause( );
    return true;
}

// 选择
int DoQingziClass::choose_function( ) {
    int a = 0;
    while (a != 1 && a != 2 && a != 3) {
        clearConsole( );
        std::cout << U8C(u8"请选择要生成excel表的类型：") << std::endl
                  << U8C(u8"1. 活动签到表") << std::endl;
        std::cout << U8C(u8"2. 出勤记录表") << std::endl;
        std::cout << U8C(u8"3. 青字班报名") << std::endl;
        std::cout << U8C(u8"请选择（ 输入 1 或者 2 或者 3 后按下 Enter键 ）：");
        std::cin >> a;
        if (a == 1 || a == 2 || a == 3) {
            return a;
        } else {
            std::cout << U8C(u8"你的输入错误，请输入 1 或者 2 或者 3 后按下 Enter键 ")
                      << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

// @brief 加载全学员表的函数
void DoQingziClass::load_personnel_information_list( ) {
    std::cout << U8C(u8"加载全学员信息表...") << std::endl;
    // lambda函数定义========================================================================/
    /*
     * @brief 保存标准的人员信息
     * @param 二维向量用于储存表格信息
     * @param 青字班的名字
     */
    auto save_information_std =
        [&](const table< std::string > &sh, std::string cn) -> void {
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
            //  std::cout << sysdcode_to_utf8("加载std") << std::endl;
            for (size_t colIndex = 0; colIndex < sh[rowIndex].size( ); colIndex++) {
                if (sh[0][colIndex] == U8C(u8"姓名")) {
                    per.name = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == U8C(u8"性别")) {
                    per.gender = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == U8C(u8"年级")) {
                    per.grade = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == U8C(u8"学号")) {
                    per.studentID = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == U8C(u8"政治面貌")) {
                    per.politicaloutlook = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == U8C(u8"学院")) {
                    per.academy = sh[rowIndex][colIndex];
                } else if (sh[0][colIndex] == U8C(u8"专业")) {
                    per.majors = sh[rowIndex][colIndex];
                } else if ((sh[0][colIndex] == U8C(u8"电话"))
                           || (sh[0][colIndex] == U8C(u8"联系方式"))
                           || (sh[0][colIndex] == U8C(u8"联系电话"))
                           || (sh[0][colIndex] == U8C(u8"电话号码"))) {
                    per.phonenumber = sh[rowIndex][colIndex];
                } else if ((sh[0][colIndex] == U8C(u8"QQ号"))
                           || (sh[0][colIndex] == U8C(u8"qq号"))
                           || (sh[0][colIndex] == U8C(u8"qq"))
                           || (sh[0][colIndex] == U8C(u8"QQ"))) {
                    per.qqnumber = sh[rowIndex][colIndex];
                } else {
                    per.otherInformation[sh[0][colIndex]] = sh[rowIndex][colIndex];
                }
            }
            // 推送之前检查
            if (per.name.size( ) == 0)
                continue;
            else
                personStd_.push_back(per);
        }
    };
    //=======================================================================================/

    std::cout << std::endl
              << U8C(u8"读取全学院名单...") << std::endl
              << std::endl;



    file::get_filepath_from_folder(
        className_,
        filePathAndName_,
        "./input/all/",
        list< std::string >{ ".xlsx" });

    // 按文件读取每个青字班的信息表
    for (auto it_className = className_.begin( ), it_filePathAndName = filePathAndName_.begin( );
         it_className != className_.end( ) && it_filePathAndName != filePathAndName_.end( );
         it_className++, it_filePathAndName++) {
        // 保存读取到的表格
        table< std::string > sheet;
        file::load_sheet_from_xlsx(sheet, *it_filePathAndName);
        save_information_std(sheet, *it_className);
    }
}



/* ======================================================================================================================= */


// @brief 控制生成签到表的函数
void DoQingziClass::attendance( ) {
    /* 1.加载全学员表 ===================================================================== */
    load_personnel_information_list( );

    stats_applicants( );
    save_attendanceSheet( );
    if (unknownAppPerson_.size( ) >= 1) {
        std::cout << std::endl
                  << std::endl
                  << "\033[43;30mWARNING!!!\033[0m" << std::endl;
        std::cout << "\033[43;30m";
        std::cout << "###ATTENTION### ";
        std::cout << U8C(u8"以下的人员不在全学员名单中");
        std::cout << " ###ATTENTION###";
        std::cout << std::endl;
        for (auto it_unknownPerson = unknownAppPerson_.begin( );
             it_unknownPerson != unknownAppPerson_.end( );
             it_unknownPerson++) {
            if (it_unknownPerson->personStd.ifcheck == false) {
                std::cout << "Unknown:  ";
                std::cout << it_unknownPerson->personStd.classname << "    ";
                std::cout << it_unknownPerson->personStd.name << "    ";
                if (it_unknownPerson->personStd.studentID.size( ) != 0) {
                    std::cout << it_unknownPerson->personStd.studentID << "    ";
                } else {
                    std::cout << U8C(u8"？学号不存在？ ");
                }
                std::cout << std::endl;

                for (size_t i = 0; i < it_unknownPerson->likelyPerson.size( ); i++) {
                    std::cout << "-likely:  ";
                    std::cout << it_unknownPerson->likelyPerson[i].classname << "    ";
                    std::cout << it_unknownPerson->likelyPerson[i].name << "    ";
                    std::cout << it_unknownPerson->likelyPerson[i].studentID << "    ";
                    std::cout << it_unknownPerson->likelyRate[i] << std::endl;
                }
                std::cout << std::endl;
            }
        }
        std::cout << "###ATTENTION### ";
        std::cout << U8C(u8"以上的人员不在全学员名单中");
        std::cout << " ###ATTENTION###";
        std::cout << "\033[0m";
        std::cout << std::endl
                  << std::endl
                  << std::endl;
    }

    std::cout << U8C(u8"已完成签到表输出，请在 output/app_out 中查看！")
              << std::endl
              << std::endl;

    save_storageSheet( );
    std::cout << U8C(u8"已完成相关数据的缓存...") << std::endl
              << std::endl;
    save_unknown_person(unknownAppPerson_);
    std::cout << U8C(u8"未知的人员信息已输出到 output/unknown.xlsx 中")
              << std::endl
              << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return;
}

// @brief 统计报名人员
void DoQingziClass::stats_applicants( ) {
    list< std::string > app_classname;          // 班级名称
    list< std::string > app_filePathAndName;    // applicationSheet的excel文件的路径
    list< DefLine >     app_person;             // 定义从报名表中获得的人员信息

    int a = 0;
    while (a != 1 && a != 2) {
        clearConsole( );
        std::cout << std::endl
                  << U8C(u8"请选择生成方式：") << std::endl
                  << U8C(u8"1.生成部分人员的签到表") << std::endl
                  << U8C(u8"2.生成所有人员的签到表") << std::endl;
        std::cout << U8C(u8"请选择（ 输入 1 或者 2 后按下 Enter键 ）：");
        std::cin >> a;
        if (a == 1) {
            break;
        } else if (a == 2) {
            for (auto &per : personStd_) {
                per.ifsign = true;
            }
            return;
        } else {
            std::cout << U8C(u8"你的输入错误，请输入 1 或者 2 后按下 Enter键 ") << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    // lambda函数定义========================================================================/
    /*
     * @brief 保存报名表中的信息
     * @param 表格信息
     * @param 班级名称
     */
    auto extract_application_to_vector =
        [&app_person](const table< std::string > &sh, std::string cn) -> void {
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
              << U8C(u8"读取各班的报名表...") << std::endl;
    /*
     * 为什么不用DefFolder类： [@lscatfish]我不想改了，还有两个向量信息对齐的原因
     * DefFolder
     */
    file::get_filepath_from_folder(
        app_classname,
        app_filePathAndName,
        "./input/app/",
        list< std::string >{ ".xlsx" });
    std::cout << std::endl;

    for (auto it_app_filepath = app_filePathAndName.begin( ), it_app_classname = app_classname.begin( );
         it_app_filepath != app_filePathAndName.end( ) && it_app_classname != app_classname.end( );
         it_app_filepath++, it_app_classname++) {
        // 保存读取到的表格
        table< std::string > sheet;
        file::load_sheet_from_xlsx(sheet, *it_app_filepath);
        extract_application_to_vector(sheet, *it_app_classname);
    }

    /* 报名，标定人员 */
    for (auto it_app_person = app_person.begin( ); it_app_person != app_person.end( ); it_app_person++) {
        list< DefPerson >::iterator it_search = personStd_.end( );    // 赋值到哨兵迭代器
        search_person(it_search, *it_app_person);
        if (it_search != personStd_.end( )) {    // 说明搜索到了
            it_search->ifsign      = true;       // 已报名
            it_app_person->ifcheck = true;       // 这里的ifcheck说明报了名的人已经匹配
        } else {
            it_app_person->ifcheck = false;    // 没有搜索到
        }
    }

    // 标定没有搜索到的人
    for (auto it_app_person = app_person.begin( );
         it_app_person != app_person.end( );
         it_app_person++) {
        if (it_app_person->ifcheck == false) {
            if (it_app_person->information[U8C(u8"姓名")].size( ) != 0) {
                DefUnknownPerson unP;
                unP.personLine = *it_app_person;
                trans_line_to_person(unP.personLine, unP.personStd);
                unknownAppPerson_.push_back(unP);
            }
        }
    }
    // 模糊匹配没有搜到的人
    for (auto &unknownPerson : unknownAppPerson_) {
        fuzzy::search_for_person(unknownPerson.likelyPerson, unknownPerson.likelyRate, unknownPerson.personStd, personStd_);
    }

    // 查验相似度
    for (auto itun = unknownAppPerson_.begin( ); itun != unknownAppPerson_.end( );) {
        bool next_it = true;
        for (size_t i = 0; i < itun->likelyPerson.size( ); i++) {
            if (itun->likelyRate[i] == 1.0) {
                auto it_search = personStd_.end( );
                search_person(it_search, itun->likelyPerson[i]);
                if (it_search != personStd_.end( )) {
                    it_search->ifsign = true;
                    itun              = unknownAppPerson_.erase(itun);
                    next_it           = false;
                    break;
                }
            }
        }
        if (next_it)
            itun++;
    }
}

// @brief 保存签到表
void DoQingziClass::save_attendanceSheet( ) {
    for (auto it_classname = className_.begin( ); it_classname != className_.end( ); it_classname++) {
        std::string sheetTitle = *it_classname + U8C(u8"签到表");
        std::string sheetPath  = "./output/app_out/" + (*it_classname) + ".xlsx";

        table< std::string > sheet = {
            { U8C(u8"序号"), U8C(u8"姓名"),
              U8C(u8"学号"), U8C(u8"签到") }
        };
        int serialNum = 1;
        for (auto it_person = personStd_.begin( ); it_person != personStd_.end( ); it_person++) {
            if (it_person->classname == *it_classname && it_person->ifsign == true) {
                list< std::string > aRow;
                aRow.push_back(std::to_string(serialNum));
                aRow.push_back(it_person->name);
                aRow.push_back(it_person->studentID);
                aRow.push_back("");
                sheet.push_back(aRow);
                serialNum++;
            }
        }
        file::save_attSheet_to_xlsx(sheet, sheetPath, sheetTitle);
    }
}


/* ======================================================================================================================= */


// @brief 控制生成签到考勤表的函数
void DoQingziClass::statistics( ) {
    /* 1.加载全学员表 ===================================================================== */
    load_personnel_information_list( );
    ppocr::Init( );
    load_storageSheet( );
    // 制作考勤统计表
    /* std::cout << std::endl
               << u8"此功能还在开发中..." << std::endl;*/
    stats_checkinners( );
    save_statisticsSheet( );
    if (unknownAttPerson_.size( ) > 0) {
        std::cout << std::endl
                  << std::endl
                  << "\033[43;30mWARNING!!!\033[0m" << std::endl;
        std::cout << "\033[43;30m";
        std::cout << "###ATTENTION### ";
        std::cout << U8C(u8"图片中的以下人员不在全学员名单中");
        std::cout << " ###ATTENTION###";
        std::cout << std::endl;

        for (auto it_unknownPerson = unknownAttPerson_.begin( );
             it_unknownPerson != unknownAttPerson_.end( );
             it_unknownPerson++) {
            std::cout << "Unknown:  ";
            std::cout << it_unknownPerson->personStd.classname << "    ";
            std::cout << it_unknownPerson->personStd.name << "    ";
            if (it_unknownPerson->personStd.studentID.size( ) != 0) {
                std::cout << it_unknownPerson->personStd.studentID << "    ";
            } else {
                std::cout << U8C(u8"？学号不存在？ ");
            }
            std::cout << std::endl;
            for (size_t i = 0; i < it_unknownPerson->likelyPerson.size( ); i++) {
                std::cout << "-likely:  ";
                std::cout << it_unknownPerson->likelyPerson[i].classname << "    ";
                std::cout << it_unknownPerson->likelyPerson[i].name << "    ";
                std::cout << it_unknownPerson->likelyPerson[i].studentID << "    ";
                std::cout << it_unknownPerson->likelyRate[i] << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << "###ATTENTION### ";
        std::cout << U8C(u8"以上的人员不在全学员名单中");
        std::cout << " ###ATTENTION###";
        std::cout << "\033[0m";
        std::cout << std::endl
                  << std::endl
                  << std::endl;
    }
    std::cout << U8C(u8"已完成线下签到汇总表的输出，请在 output/att_out 中查看！")
              << std::endl
              << std::endl;
    save_unknown_person(unknownAttPerson_);
    std::cout << U8C(u8"未知的人员信息已输出到 output/unknown.xlsx 中")
              << std::endl
              << std::endl;
    ppocr::Uninit( );
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return;
}

// @brief 制作考勤统计表
void DoQingziClass::stats_checkinners( ) {

    /*  ===================================================================
     *				================
     *				||函数工作说明||
     *				================
     * @brief 路径"./input/att/"中拉取图片的路径与名称
     * 图片的命名规则：青x班1   青x班2    (末尾的数字是某班级签到表照片的第i张)
     *
     * ----------------------------------------------------------------- */

    std::map< std::string, list< std::string > > classname__filePathAndName;    // 班级名称与各班的签到表的匹配
    list< std::string >                          att_fileName;                  // 图片的文件名(无后缀)
    list< std::string >                          att_filePathAndName;           // 图片-签到表文件的路径
    std::vector< DefPerson >                     att_person;                    // 定义从签到表中获得的人员信息

    // lambda函数定义========================================================================================/
    /*
     * @brief 提取sheet（表格）的信息到att_person
     * @param sh 表格信息
     * @param cn 班级名称
     */
    auto extract_attendance_to_vector =
        [&att_person](const table< std::string > &sh, const std::string &cn) -> void {
        std::vector< DefLine > att_person_line;    // 人员行信息
        size_t                 rowHeader = 1;      // 默认表头在第一行
        list< std::string >    headerLine;         // 表头

        // 首先找到 表头 所在的行
        for (size_t rowIndex = 0; rowIndex < sh.size( ); rowIndex++) {
            for (size_t colIndex = 0; colIndex < sh[rowIndex].size( ); colIndex++) {
                if (sh[rowIndex][colIndex] == U8C(u8"姓名")
                    || sh[rowIndex][colIndex] == U8C(u8"序号")
                    || sh[rowIndex][colIndex] == U8C(u8"学号")
                    || sh[rowIndex][colIndex] == U8C(u8"签到")
                    || sh[rowIndex][colIndex] == U8C(u8"签字")) {
                    rowHeader = rowIndex;
                    break;
                }
            }
        }
        // 保存表头信息
        for (size_t col = 0; col < sh[rowHeader].size( ); col++) {
            headerLine.push_back(sh[rowHeader][col]);
        }

        // 加载全套的表格,解析是否签字
        for (size_t rowIndex = 0; rowIndex < sh.size( ); rowIndex++) {
            // 跳过表头
            if (rowIndex == rowHeader) continue;

            // 先检测此行是否有效
            size_t emptyCell = 0;    // 空的单元格数量,如果空单元格数量 >=2 则说明此行无效
            for (size_t colIndex = 0; colIndex < sh[rowIndex].size( ); colIndex++) {
                if (sh[rowIndex][colIndex].size( ) == 0)
                    emptyCell++;
            }
            if (emptyCell >= 2) continue;

            // 首先按行保存
            DefLine perline;
            perline.classname = cn;
            for (size_t colIndex = 0; colIndex < sh[rowHeader].size( ); colIndex++) {
                if (colIndex < sh[rowIndex].size( ))    // 仅当此行存在时
                    perline.information[sh[rowHeader][colIndex]] = sh[rowIndex][colIndex];
                else
                    perline.information[sh[rowHeader][colIndex]] = "";
            }

            // 顺便解析是否签字
            if (perline.information.find(U8C(u8"签字")) != perline.information.end( )) {
                if (perline.information[U8C(u8"签字")].size( ) != 0)
                    perline.ifcheck = true;
                else
                    perline.ifcheck = false;
                perline.information.erase(U8C(u8"签字"));
            } else if (perline.information.find(U8C(u8"签到")) != perline.information.end( )) {
                if (perline.information[U8C(u8"签到")].size( ) != 0)
                    perline.ifcheck = true;
                else
                    perline.ifcheck = false;
                perline.information.erase(U8C(u8"签到"));
            } else {
                perline.ifcheck = false;
            }
            att_person_line.push_back(perline);
        }

        // 解析att_person_line中的内容到att_person
        for (const auto &perline : att_person_line) {
            DefPerson per;
            trans_line_to_person(perline, per);
            att_person.push_back(per);
        }
    };

    /*
     * @brief 打赢sheet的结果
     * @param sh 表格
     */
    auto sheet_printer = [](const table< std::string > &sh) -> void {
        std::cout << std::endl
                  << std::endl;
        for (size_t r = 0; r < sh.size( ); r++) {
            for (size_t c = 0; c < sh[r].size( ); c++) {
                std::cout << sh[r][c] << "    ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl
                  << std::endl;
    };

    // ===================================================================================================/

    // 1.拉取文件夹中的所有照片的地址
    list< std::string > u8path;
    file::get_imgpath_from_folder(
        att_filePathAndName,
        att_fileName,
        u8path,
        "./input/att_imgs/",
        list< std::string >{ ".jpg", ".png", ".jpeg", ".tiff", ".tif ",
                             ".jpe", ".bmp", ".dib", ".webp", ".raw" });

    // 2.解析班级的名字与名单的数量，储存到classname__filePathAndName中
    for (auto it_att_fileName = att_fileName.begin( ), it_att_filePathAndName = att_filePathAndName.begin( );
         it_att_fileName != att_fileName.end( ) && it_att_filePathAndName != att_filePathAndName.end( );
         it_att_fileName++, it_att_filePathAndName++) {
        auto [chinese, number] = split_chinese_and_number(*it_att_fileName);
        classname__filePathAndName[chinese].push_back(*it_att_filePathAndName);
    }

    // 3.解析每个班的图片
    for (auto it_cfPAN = classname__filePathAndName.begin( );
         it_cfPAN != classname__filePathAndName.end( );
         it_cfPAN++) {
        table< std::string > sh;
        for (size_t i = 0; i < it_cfPAN->second.size( ); i++) {
            table< std::string > partSh;
            img::load_sheet_from_img(partSh, it_cfPAN->second[i]);
            sh = mergeMultipleSheets(sh, partSh);
            std::cout << U8C(u8"融合结束") << std::endl;
        }
        // 打印结果
        sheet_printer(sh);
        extract_attendance_to_vector(sh, it_cfPAN->first);
    }

    // 4.解析人员的签到情况到全人员表
    for (const auto &attper : att_person) {
        auto it_perstd = personStd_.end( );    // 哨兵值
        search_person(it_perstd, attper);
        if (it_perstd != personStd_.end( )) {
            it_perstd->ifcheck = attper.ifcheck;
        } else {
            // 尚未匹配到
            DefUnknownPerson un;
            un.personStd = attper;
            bool ifpush  = true;
            // 模糊匹配没有搜索到的人
            if (fuzzy::search_for_person(un.likelyPerson, un.likelyRate, un.personStd, personStd_)) {
                // 查验相似度
                for (size_t i = 0; i < un.likelyRate.size( ); i++) {
                    if (un.likelyRate[i] == 1.0) {
                        auto it_search = personStd_.end( );
                        search_person(it_search, un.likelyPerson[i]);
                        if (it_search != personStd_.end( )) {
                            it_search->ifcheck = attper.ifcheck;
                            ifpush             = false;
                            break;
                        }
                    }
                }
            }
            if (ifpush)
                unknownAttPerson_.push_back(un);
        }
    }
}

// @brief 保存签到考勤表
void DoQingziClass::save_statisticsSheet( ) {
    /* ==================================================================================
     * 制表方式
     * 标题xxxx-xxxx学年x（春\秋）季学期第x次集中培训青x班学员线下签到汇总
     * |姓名|学号|学员|联系|方式|签到|备注|
     * ================================================================================= */
    for (auto it_className = className_.begin( ); it_className != className_.end( ); it_className++) {
        std::string sheetTitle    = *it_className + U8C(u8"学员线下签到汇总");
        std::string sheetSavePath = "./output/att_out/" + *it_className + ".xlsx";

        table< std::string > sheet = {
            { U8C(u8"姓名"), U8C(u8"学号"),
              U8C(u8"学院"), U8C(u8"联系方式"),
              U8C(u8"签到"), U8C(u8"备注") }
        };

        for (auto it_person = personStd_.begin( ); it_person != personStd_.end( ); it_person++) {
            if (it_person->classname == *it_className) {
                list< std::string > line;
                line.push_back(it_person->name);
                line.push_back(it_person->studentID);
                line.push_back(it_person->academy);
                line.push_back(it_person->phonenumber);
                if (it_person->ifcheck) {
                    line.push_back(U8C(u8"已签到"));
                } else {
                    if (it_person->ifsign)    // 没有到场，但是报名
                    {
                        line.push_back(U8C(u8"缺席"));
                    } else {
                        line.push_back("");
                    }
                }
                if (it_person->ifsign) {
                    line.push_back("");
                } else {
                    line.push_back(U8C(u8"未报名"));
                }
                sheet.push_back(line);
            }
        }
        file::save_sttSheet_to_xlsx(sheet, sheetSavePath, sheetTitle);
    }
}


/* ======================================================================================================================= */


// @brief 青字班报名
void DoQingziClass::registration( ) {
    file::DefFolder *aFolder = new file::DefFolder(file::_INPUT_SIGN_QC_ALL_DIR_);
    if (!aFolder) {
        std::cout << U8C(u8"内存分配失败") << std::endl;
    }

    list< std::string > paths = aFolder->get_filepath_list(list< std::string >{ ".docx", ".DOCX" });    // 文件路径
    // 解析docx文件
    if (paths.size( ) != 0) {
        for (const auto &p : paths) {
            docx::DefDocx aDocx(p);
            personStd_.push_back(aDocx.get_person( ));
        }
    }

    // 处理pdf
    file::DefFolder pdfFiles(*aFolder, list< std::string >{ ".pdf", ".PDF" });
    paths = pdfFiles.get_u8filepath_list( );    // 文件路径
    // 解析pdf文件
    if (paths.size( ) != 0) {
        for (const auto &p : paths) {
            pdf::DefPdf aPdf(p);
            if (aPdf.isOKed( ) && aPdf.get_sheet_type( ) == pdf::DefPdf::SheetType::Committee) {
                DefPerson per = aPdf.get_person( );
                auto      it  = personStd_.end( );
                search_person(it, per);
                if (it != personStd_.end( )) {
                    it->otherInformation[U8C(u8"文件地址")] += (p + " ; ");
                    it->ifsign       = true;
                    it->signPosition = per.signPosition;
                    pdfFiles.erase_with(p);
                } else {
                    per.ifsign                              = true;
                    per.otherInformation[U8C(u8"文件地址")] = p;
                    per.otherInformation[U8C(u8"报名方式")] = U8C(u8"组织推荐");
                    per.otherInformation[U8C(u8"备注")] = U8C(u8"未找到docx文档");
                    personStd_.push_back(per);
                    pdfFiles.erase_with(p);
                }
            }
        }
    }

    save_registrationSheet( );

    // 筛选pdf文件
    std::cout << std::endl
              << U8C(u8"复制了")
              << pdfFiles.copy_files_to(file::_OUTPUT_SIGN_QC_PDF_DIR_)
              << U8C(u8"份pdf文件到") << file::_OUTPUT_SIGN_QC_PDF_DIR_ << std::endl;

    std::cout << std::endl
              << U8C(u8"青字班报名表已输出到 ./output/sign_for_QingziClass_out/报名.xlsx 中...")
              << std::endl;
    delete aFolder;
    aFolder = nullptr;    // 还是不要有野指针
}

// @brief 保存青字班的报名表
void DoQingziClass::save_registrationSheet( ) {
    table< std::string > sh{
        { U8C(u8"序号"), U8C(u8"姓名"),
          U8C(u8"性别"), U8C(u8"民族"),
          U8C(u8"年级"), U8C(u8"学号"),
          U8C(u8"政治面貌"), U8C(u8"学院"),
          U8C(u8"专业"), U8C(u8"学生职务"),
          U8C(u8"社团"), U8C(u8"联系电话"),
          U8C(u8"QQ号"), U8C(u8"邮箱"),
          U8C(u8"报名青字班"), U8C(u8"是否报名班委"),
          U8C(u8"应聘岗位"),
          U8C(u8"报名方式"), U8C(u8"备注"),
          U8C(u8"个人简介"), U8C(u8"个人特长"),
          U8C(u8"工作经历"), U8C(u8"获奖情况"),
          U8C(u8"文件地址") }
    };                 // 表格
    int serial = 1;    // 序号
    for (auto &p : personStd_) {
        if (p.name.size( ) == 0) continue;
        list< std::string > line;
        line.push_back(std::to_string(serial));
        line.push_back(p.name);
        line.push_back(p.gender);
        line.push_back(p.ethnicity);
        line.push_back(p.grade);
        line.push_back(p.studentID);
        line.push_back(p.politicaloutlook);
        line.push_back(p.academy);
        line.push_back(p.majors);
        line.push_back(p.position);
        line.push_back(p.club);
        line.push_back(p.phonenumber);
        line.push_back(p.qqnumber);
        line.push_back(p.email);
        line.push_back(p.classname);
        if (p.ifsign)
            line.push_back(U8C(u8"是"));
        else
            line.push_back(U8C(u8"否"));
        line.push_back(p.signPosition);
        if (p.otherInformation.find(U8C(u8"报名方式")) != p.otherInformation.end( ))
            line.push_back(p.otherInformation[U8C(u8"报名方式")]);
        else
            line.push_back("");
        if (p.otherInformation.find(U8C(u8"备注")) != p.otherInformation.end( ))
            line.push_back(p.otherInformation[U8C(u8"备注")]);
        else
            line.push_back("");
        if (p.otherInformation.find(U8C(u8"个人简介")) != p.otherInformation.end( ))
            line.push_back(p.otherInformation[U8C(u8"个人简介")]);
        else
            line.push_back("");
        if (p.otherInformation.find(U8C(u8"个人特长")) != p.otherInformation.end( ))
            line.push_back(p.otherInformation[U8C(u8"个人特长")]);
        else
            line.push_back("");
        if (p.otherInformation.find(U8C(u8"工作经历")) != p.otherInformation.end( ))
            line.push_back(p.otherInformation[U8C(u8"工作经历")]);
        else
            line.push_back("");
        if (p.otherInformation.find(U8C(u8"获奖情况")) != p.otherInformation.end( ))
            line.push_back(p.otherInformation[U8C(u8"获奖情况")]);
        else
            line.push_back("");
        if (p.otherInformation.find(U8C(u8"文件地址")) != p.otherInformation.end( ))
            line.push_back(p.otherInformation[U8C(u8"文件地址")]);
        else
            line.push_back("");

        sh.push_back(line);
        serial++;
    }
    file::save_registrationSheet_to_xlsx(sh);
}


/*
 * @brief 搜索，从全人员名单中搜素目标人员信息
 * @param _it_output 总名单的一个迭代器
 * @param _targetPerson 目标的人员信息
 * @note 可以考虑怎么优化这两个search函数
 * @shit if很多吧，慢慢看  (^-^)
 */
void DoQingziClass::search_person(list< DefPerson >::iterator &_it_output, DefPerson _targetPerson) {
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
 * @param _it_output 总名单的一个迭代器
 * @param _targetPerson 目标的人员信息
 * @note 可以考虑怎么优化这两个search函数
 * @shit if很多吧，慢慢看  (^-^)
 */
void DoQingziClass::search_person(list< DefPerson >::iterator &_it_output, DefLine _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if (_targetPerson.classname == it_all->classname
                && _targetPerson.information[U8C(u8"姓名")] == it_all->name) {
                if (_targetPerson.information.find(U8C(u8"学号")) != _targetPerson.information.end( )) {
                    if (compare_studentID(_targetPerson.information[U8C(u8"学号")], it_all->studentID)) {
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
            if (_targetPerson.information[U8C(u8"姓名")] == it_all->name) {
                if (_targetPerson.information.find(U8C(u8"学号")) != _targetPerson.information.end( )) {
                    if (compare_studentID(_targetPerson.information[U8C(u8"学号")], it_all->studentID)) {
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
 * @param _inperLine 一行信息
 * @param _outperStd 标准的人员信息
 */
void DoQingziClass::trans_line_to_person(const DefLine &_inperLine, DefPerson &_outperStd) {
    DefPerson per;
    per.classname = _inperLine.classname;
    per.ifcheck   = _inperLine.ifcheck;
    per.ifsign    = _inperLine.ifcheck;
    for (auto it_inperLine = _inperLine.information.begin( );
         it_inperLine != _inperLine.information.end( );
         it_inperLine++) {
        if (it_inperLine->first == U8C(u8"姓名")) {
            per.name = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"性别")) {
            per.gender = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"年级")) {
            per.grade = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"学号")) {
            per.studentID = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"政治面貌")) {
            per.politicaloutlook = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"学院")) {
            per.academy = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"专业")) {
            per.majors = it_inperLine->second;
        } else if ((it_inperLine->first == U8C(u8"电话"))
                   || (it_inperLine->first == U8C(u8"联系方式"))
                   || (it_inperLine->first == U8C(u8"联系电话"))
                   || (it_inperLine->first == U8C(u8"电话号码"))) {
            per.phonenumber = it_inperLine->second;
        } else if ((it_inperLine->first == U8C(u8"QQ号"))
                   || (it_inperLine->first == U8C(u8"qq号"))
                   || (it_inperLine->first == U8C(u8"qq"))
                   || (it_inperLine->first == U8C(u8"QQ"))) {
            per.qqnumber = it_inperLine->second;
        } else if ((it_inperLine->first == U8C(u8"所任职务"))
                   || (it_inperLine->first == U8C(u8"职务"))
                   || (fuzzy::search_substring(it_inperLine->first, U8C(u8"职务")))
                   || (fuzzy::search_substring(it_inperLine->first, U8C(u8"所任职务")))) {
            per.position = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"邮箱")) {
            per.email = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"民族")) {
            per.ethnicity = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"社团")) {
            if (it_inperLine->second.size( ) == 0) {
                per.club = U8C(u8"无");
            } else {
                per.club = it_inperLine->second;
            }
        } else if ((it_inperLine->first == U8C(u8"报名青字班"))
                   || (it_inperLine->first == U8C(u8"青字班"))
                   || (fuzzy::search_substring(it_inperLine->first, U8C(u8"青字班")))) {
            per.classname = it_inperLine->second;
        } else if (it_inperLine->first == U8C(u8"应聘岗位")) {
            per.signPosition = it_inperLine->second;
        } else {
            per.otherInformation[it_inperLine->first] = it_inperLine->second;
        }
    }
    _outperStd = per;
}

/*
 * @brief 标准人员信息转化为一行信息
 * @param _inperStd 标准的人员信息
 * @param _outperLine 一行信息
 * @note 这个函数好像没怎么用到
 */
void DoQingziClass::trans_person_to_line(const DefPerson &_inperStd, DefLine &_outperLine) {
    DefLine per;
    per.classname                      = _inperStd.classname;
    per.information[U8C(u8"姓名")]     = _inperStd.name;
    per.information[U8C(u8"性别")]     = _inperStd.gender;
    per.information[U8C(u8"年级")]     = _inperStd.grade;
    per.information[U8C(u8"学号")]     = _inperStd.studentID;
    per.information[U8C(u8"政治面貌")] = _inperStd.politicaloutlook;
    per.information[U8C(u8"学院")]     = _inperStd.academy;
    per.information[U8C(u8"专业")]     = _inperStd.majors;
    per.information[U8C(u8"QQ号")]     = _inperStd.qqnumber;
    per.ifcheck                        = _inperStd.ifcheck;
    per.ifsign                         = _inperStd.ifcheck;
    if (_inperStd.otherInformation.size( ) != 0) {
        for (auto it_in = _inperStd.otherInformation.begin( );
             it_in != _inperStd.otherInformation.end( );
             it_in++) {
            per.information[it_in->first] = it_in->second;
        }
    }
    _outperLine = per;
}


// @brief 缓存全部报名的人员
void DoQingziClass::save_storageSheet( ) {
    // 按照 班级  姓名  学号  的方式保存
    // 仅保存报名的人员s
    table< std::string > sh;
    // 制表
    for (const auto &per : personStd_) {
        if (!per.ifsign) continue;
        list< std::string > line;
        line.push_back(per.classname);
        line.push_back(per.name);
        line.push_back(per.studentID);
        sh.push_back(line);
    }
    file::save_storageSheet_to_xlsx(sh);
}

// @brief 加载缓存的全部报名的人员
void DoQingziClass::load_storageSheet( ) {
    std::cout << std::endl
              << U8C(u8"加载缓存文件...") << std::endl
              << std::endl;

    // 按照 班级  学号  的方式读取
    table< std::string > sh;
    file::load_storageSheet_from_xlsx(sh);

    for (const auto &line : sh) {
        DefPerson per;
        per.classname = line[0];
        per.name      = line[1];
        per.studentID = line[2];
        auto it       = personStd_.end( );
        search_person(it, per);
        if (it != personStd_.end( )) {
            it->ifsign = true;
            continue;
        } else {
            /*报错*/
            /*按道理来说不应该报错*/
        }
    }
}

/*
 * @brief 保存尚未搜索到的成员
 * @param _in_unLists 未搜索到的成员列表
 */
void DoQingziClass::save_unknown_person(const list< DefUnknownPerson > &_in_unLists) {
    table< std::string > sh = {
        { "", U8C(u8"班级"), U8C(u8"姓名"),
          U8C(u8"学号"), U8C(u8"相似度") }
    };
    for (auto &unPer : _in_unLists) {
        list< std::string > line1;
        line1.push_back("*UNKNOWN");
        line1.push_back(unPer.personStd.classname);
        line1.push_back(unPer.personStd.name);
        if (unPer.personStd.studentID.size( ) != 0) {
            line1.push_back(unPer.personStd.studentID);
        } else {
            line1.push_back(U8C(u8"未知"));
        }
        line1.push_back("");
        sh.push_back(line1);
        for (size_t i = 0; i < unPer.likelyPerson.size( ); i++) {
            list< std::string > line2;
            line2.push_back("-LIKELY");
            line2.push_back(unPer.likelyPerson[i].classname);
            line2.push_back(unPer.likelyPerson[i].name);
            line2.push_back(unPer.likelyPerson[i].studentID);
            line2.push_back(std::to_string(unPer.likelyRate[i]));
            sh.push_back(line2);
        }
    }
    file::save_unknownPerSheet_to_xlsx(sh);
}