// Author: lscatfish

#include <algorithm>
#include <basic.hpp>
#include <chrono>
#include <chstring.hpp>
#include <console.h>
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
    int outWhichSheet = choose_function(3, myList< std::string >{
                                               U8C(u8"请选择要生成excel表的类型："),
                                               U8C(u8"1. 活动签到表"),
                                               U8C(u8"2. 出勤记录表"),
                                               U8C(u8"3. 青字班报名") });    // 生成哪一张表

    std::cout << std::endl
              << std::endl;

    /* 4.加载签到表或是出勤记录表 ============================================================= */
    if (outWhichSheet == 1) {
        file::DefFolder h(file::_OUTPUT_APP_DIR_, false);
        h.delete_with( );
        std::this_thread::sleep_for(std::chrono::seconds(1));
        console::clear_console( );
        applicants( );
    } else if (outWhichSheet == 2) {
        file::DefFolder i(file::_OUTPUT_ATT_DIR_, false);
        i.delete_with( );
        std::this_thread::sleep_for(std::chrono::seconds(1));
        console::clear_console( );
        statistics( );
    } else if (outWhichSheet == 3) {
        file::DefFolder f(file::_OUTPUT_SIGN_QC_UNPDF_DIR_, false);
        file::DefFolder g(file::_OUTPUT_SIGN_QC_CMT_DIR_, false);
        f.delete_with( );
        g.delete_with( );
        std::this_thread::sleep_for(std::chrono::seconds(1));
        console::clear_console( );
        registration( );
    }
}

// 自检程序
bool DoQingziClass::self_check( ) {
    console::clear_console( );
    std::cout << U8C(u8"启动自检程序......") << std::endl
              << std::endl;
    std::cout << U8C(u8"检测工作区的文件夹：") << std::endl;
    const myList< std::string > ws_pathList{
        "./models/",
        file::_INPUT_ALL_DIR_,
        file::_INPUT_APP_DIR_,
        file::_INPUT_ATT_IMGS_DIR_,
        file::_INPUT_SIGN_QC_ALL_DIR_,
        file::_OUTPUT_APP_DIR_,
        file::_OUTPUT_ATT_DIR_,
        file::_OUTPUT_SIGN_QC_DIR_,
        file::_OUTPUT_SIGN_QC_UNPDF_DIR_,
        file::_OUTPUT_SIGN_QC_CMT_DIR_,
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
    const myList< std::string > md_pathList{
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
        std::cout << U8C(u8"模型字典库错误：") << ppocr::_ppocrDir_.rec_char_dict_path << std::endl;
        pause( );
        return false;
    }
    std::cout << U8C(u8"模型文件检测通过...") << std::endl
              << std::endl;

    // 检测是否被占用
    std::cout << U8C(u8"检测工作区文件夹是否被占用") << std::endl;
    file::DefFolder ipt(file::_INPUT_DIR_, false);
    file::DefFolder opt(file::_OUTPUT_DIR_, false);
    file::DefFolder stg(file::_STORAGE_DIR_, false);
    // 拆分条件，确保每个函数都被调用
    auto ipt_res = ipt.check_occupied_utf8(false, true);
    auto opt_res = opt.check_occupied_utf8(false, true);    // 强制调用
    auto stg_res = stg.check_occupied_utf8(false, true);    // 强制调用
    // 为什么这么写：[@lscatfish]因为编译器会把check_occupied_utf8.size优化掉（+!_!+）
    if (ipt_res.size( ) != 0 && opt_res.size( ) != 0 && stg_res.size( ) != 0) {
        std::cout << U8C(u8"工作区文件夹被占用，程序终止！！！") << std::endl;
        return false;
    } else {
        std::cout << U8C(u8"工作区文件夹未被占用，检测通过...") << std::endl;
    }

    std::cout << std::endl
              << U8C(u8"自检完毕...") << std::endl;
    pause( );
    return true;
}


/*
 * @brief 选择函数
 * @param _chosseAll 总选项数目
 * @param _outPrint 要打印在控制台上的内容
 */
int DoQingziClass::choose_function(int _chosseAll, const myList< std::string > &_outPrint) {
    std::string a = "";
    while (true) {
        console::clear_console( );
        for (const auto &line : _outPrint)
            std::cout << line << std::endl;
        std::cout << U8C(u8"请选择（输入 1 - ") << _chosseAll << U8C(u8" 之间的整数后按下 Enter 键）：");
        std::cin >> a;
        if (is_all_digits(a) && std::stoi(a) >= 1 && std::stoi(a) <= _chosseAll) {
            return std::stoi(a);
        } else {
            std::cout << U8C(u8"你的输入错误，请输入 1 - ") << _chosseAll << U8C(u8" 之间的整数后按下 Enter 键")
                      << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
    }
    return 1;
}

// @brief 加载全学员表的函数
void DoQingziClass::load_all_personnel_information_list( ) {
    std::cout << U8C(u8"加载全学员信息表...") << std::endl;
    std::cout << std::endl
              << U8C(u8"读取全学院名单...") << std::endl
              << std::endl;

    file::DefFolder all_dir = file::DefFolder(file::_INPUT_ALL_DIR_, true);    // 我正常构造会报错
    className_              = all_dir.get_filename_list(myList< chstring >{ ".xlsx" });
    filePathAndName_        = all_dir.get_filepath_list(myList< chstring >{ ".xlsx" });

    // 按文件读取每个青字班的信息表
    for (auto it_className = className_.begin( ), it_filePathAndName = filePathAndName_.begin( );
         it_className != className_.end( ) && it_filePathAndName != filePathAndName_.end( );
         it_className++, it_filePathAndName++) {
        // 保存读取到的表格
        myTable< chstring > sheet;
        file::load_sheet_from_xlsx(sheet, *it_filePathAndName);
        // 总是相信第一行是表头
        for (size_t row = 1; row < sheet.size( ); row++) {
            DefLine   perline;
            DefPerson per;
            perline.classname = *it_className;
            for (size_t col = 0; col < sheet[0].size( ); col++) {
                perline.information[sheet[0][col]] = sheet[row][col];
            }
            trans_personline_to_person(perline, per);
            if (!per.name.empty( )) {
                per.otherInformation.clear( );
                personStd_.push_back(per);
            }
        }
    }
}


/* ======================================================================================================================= */

// @brief 控制生成签到表的函数
void DoQingziClass::applicants( ) {
    /* 1.加载全学员表 ===================================================================== */
    load_all_personnel_information_list( );

    stats_applicants( );
    save_applicantsSheet( );
    if (unknownAppPerson_.size( ) > 0)
        print_unknown_person(unknownAppPerson_, U8C(u8"请调整报名文件或者学员信息，反复运行本程序，直到没有弹出ATTENTION提示为止"));

    std::cout << U8C(u8"已完成签到表输出，请在 ") << file::_OUTPUT_APP_DIR_ << U8C(u8" 中查看！")
              << std::endl
              << std::endl;

    save_storageSheet( );
    std::cout << U8C(u8"已完成相关数据的缓存...") << std::endl
              << std::endl;
    save_unknown_person(unknownAppPerson_);
    std::cout << U8C(u8"未知的人员信息已输出到 ") << file::_OUTPUT_DIR_ << U8C(u8"unknown.xlsx 中")
              << std::endl
              << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return;
}

// @brief 统计报名人员
void DoQingziClass::stats_applicants( ) {
    myList< chstring > app_classname;    // 班级名称
    myList< chstring > app_filepath;     // applicationSheet的excel文件的路径
    myList< DefLine >  app_person;       // 定义从报名表中获得的人员信息

    int a = choose_function(2, myList< std::string >{
                                   U8C(u8"请选择生成方式："),
                                   U8C(u8"1.生成部分人员的签到表"),
                                   U8C(u8"2.生成所有人员的签到表") });
    if (a == 2) {
        for (auto &per : personStd_)
            per.ifsign = true;
        return;
    }

    // lambda函数定义========================================================================/
    /*
     * @brief 保存报名表中的信息
     * @param 表格信息
     * @param 班级名称
     */
    auto extract_application_to_vector = [&app_person](myTable< chstring > &sh, chstring cn) -> void {
        for (size_t col = 0; col < sh[0].size( ); col++)
            sh[0][col].trim_whitespace( );
        for (size_t rowIndex = 1; rowIndex < sh.size( ); rowIndex++) {
            DefLine per;
            per.classname = cn;
            for (size_t colIndex = 0;
                 colIndex < sh[rowIndex].size( ) && sh[rowIndex][colIndex].size( ) != 0;
                 colIndex++) {
                if (sh[0][colIndex].has_subchstring(U8C(u8"姓名")))
                    per.information[U8C(u8"姓名")] = sh[rowIndex][colIndex];
                else if (sh[0][colIndex].has_subchstring(U8C(u8"学号")))
                    per.information[U8C(u8"学号")] = sh[rowIndex][colIndex];
                else if (sh[0][colIndex].has_subchstring(U8C(u8"学院")))
                    per.information[U8C(u8"学院")] = sh[rowIndex][colIndex];
                else if (sh[0][colIndex].has_subchstring(U8C(u8"专业")))
                    per.information[U8C(u8"专业")] = sh[rowIndex][colIndex];
                else
                    per.information[sh[0][colIndex]] = sh[rowIndex][colIndex];
            }
            app_person.push_back(per);
        }
    };
    //=======================================================================================/
    std::cout << std ::endl
              << U8C(u8"读取各班的报名表...") << std::endl;

    file::DefFolder app_dir = file::DefFolder(file::_INPUT_APP_DIR_, true);
    app_classname           = app_dir.get_filename_list(myList< chstring >{ ".xlsx" });
    app_filepath            = app_dir.get_filepath_list(myList< chstring >{ ".xlsx" });

    std::cout << std::endl;
    // 解析保存报名的人员
    for (size_t i = 0; i < app_filepath.size( ) && i < app_classname.size( ); i++) {
        myTable< chstring > sheet;
        file::load_sheet_from_xlsx(sheet, app_filepath[i]);
        extract_application_to_vector(sheet, app_classname[i]);
    }
    // 报名，标定人员
    for (auto it_app_person = app_person.begin( ); it_app_person != app_person.end( ); it_app_person++) {
        myList< DefPerson >::iterator it_search = personStd_.end( );    // 赋值到哨兵迭代器
        if (search_person(it_search, *it_app_person)) {               // 说明搜索到了
            it_search->ifsign      = true;                            // 已报名
            it_app_person->ifcheck = true;                            // 这里的ifcheck说明报了名的人已经匹配
        } else {
            it_app_person->ifcheck = false;    // 没有搜索到
        }
    }
    // 标定没有搜索到的人
    for (auto it_app_person = app_person.begin( ); it_app_person != app_person.end( ); it_app_person++) {
        if (!it_app_person->ifcheck) {    // 没有被匹配
            if (it_app_person->information[U8C(u8"姓名")].size( ) != 0) {
                DefUnknownPerson unP;
                unP.personLine = *it_app_person;
                trans_personline_to_person(unP.personLine, unP.personStd);
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
                if (search_person(it_search, itun->likelyPerson[i])) {
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
void DoQingziClass::save_applicantsSheet( ) {
    for (auto it_classname = className_.begin( ); it_classname != className_.end( ); it_classname++) {
        chstring sheetTitle = *it_classname + U8C(u8"签到表");
        chstring sheetPath  = chstring(file::_OUTPUT_APP_DIR_) + (*it_classname) + ".xlsx";

        myTable< chstring > sheet = {
            { U8C(u8"序号"), U8C(u8"姓名"),
              U8C(u8"学号"), U8C(u8"签到") }
        };
        int serialNum = 1;
        for (auto it_person = personStd_.begin( ); it_person != personStd_.end( ); it_person++) {
            if (it_person->classname == *it_classname && it_person->ifsign == true) {
                myList< chstring > aRow;
                aRow.push_back(std::to_string(serialNum));
                aRow.push_back(it_person->name);
                aRow.push_back(it_person->studentID);
                aRow.push_back("");
                sheet.push_back(aRow);
                serialNum++;
            }
        }
        sort_table_chstring_by(sheet, 2);
        file::save_attSheet_to_xlsx(sheet, sheetPath, sheetTitle);
    }
}


/* ======================================================================================================================= */


// @brief 控制生成签到考勤表的函数
void DoQingziClass::statistics( ) {
    /* 1.加载全学员表 ===================================================================== */
    load_all_personnel_information_list( );
    ppocr::Init( );
    load_storageSheet( );
    // 制作考勤统计表
    /* std::cout << std::endl
               << u8"此功能还在开发中..." << std::endl;*/
    stats_checkinners( );
    save_statisticsSheet( );
    if (unknownAttPerson_.size( ) > 0)
        print_unknown_person(unknownAttPerson_);

    std::cout << U8C(u8"已完成线下签到汇总表的输出，请在\"") << file::_OUTPUT_ATT_DIR_ << U8C(u8"\"中查看！")
              << std::endl
              << std::endl;
    save_unknown_person(unknownAttPerson_);
    std::cout << U8C(u8"未知的人员信息已输出到\"") << file::_OUTPUT_DIR_ << U8C(u8"unknown.xlsx\"中")
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

    std::map< chstring, myList< chstring >, chstring::CompareByUTF8Desc > classname__filepath;    // 班级名称与各班的签到表的匹配
    myList< chstring >                                                    att_fileName;           // 图片的文件名(无后缀)
    myList< chstring >                                                    att_filePathAndName;    // 图片-签到表文件的路径
    myList< DefPerson >                                                   att_person;             // 定义从签到表中获得的人员信息

    // lambda函数定义========================================================================================/
    /*
     * @brief 提取sheet（表格）的信息到att_person
     * @param sh 表格信息
     * @param cn 班级名称
     */
    auto extract_attendance_to_vector = [&att_person](const myTable< chstring > &sh, const chstring &cn) -> void {
        myList< DefLine >  att_person_line;    // 人员行信息
        size_t           rowHeader = 1;      // 默认表头在第一行
        myList< chstring > headerLine;         // 表头

        // 首先找到 表头 所在的行
        for (size_t rowIndex = 0; rowIndex < sh.size( ); rowIndex++) {
            for (size_t colIndex = 0; colIndex < sh[rowIndex].size( ); colIndex++) {
                if ((sh[rowIndex][colIndex] |= U8C(u8"姓名"))
                    || (sh[rowIndex][colIndex] |= U8C(u8"序号"))
                    || (sh[rowIndex][colIndex] |= U8C(u8"学号"))
                    || (sh[rowIndex][colIndex] |= U8C(u8"签到"))
                    || (sh[rowIndex][colIndex] |= U8C(u8"签字"))) {
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
            trans_personline_to_person(perline, per);
            att_person.push_back(per);
        }
    };

    /*
     * @brief 打印sheet的结果
     * @param sh 表格
     */
    auto sheet_printer = [](const myTable< chstring > &sh) -> void {
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
    // =================================================================================================== /

    // 1.拉取文件夹中的所有照片的地址

    file::DefFolder att_imgs = file::DefFolder(file::_INPUT_ATT_IMGS_DIR_, true);
    att_filePathAndName      = att_imgs.get_filepath_list(myList< chstring >{ ".jpg", ".png", ".jpeg", ".tiff", ".tif ",
                                                                            ".jpe", ".bmp", ".dib", ".webp", ".raw" });
    att_fileName             = att_imgs.get_filename_list(myList< chstring >{ ".jpg", ".png", ".jpeg", ".tiff", ".tif ",
                                                                            ".jpe", ".bmp", ".dib", ".webp", ".raw" });

    // 2.解析班级的名字与名单的数量，储存到classname__filePathAndName中
    for (auto it_att_fileName = att_fileName.begin( ), it_att_filePathAndName = att_filePathAndName.begin( );
         it_att_fileName != att_fileName.end( ) && it_att_filePathAndName != att_filePathAndName.end( );
         it_att_fileName++, it_att_filePathAndName++) {
        auto [chinese, number] = it_att_fileName->split_chinese_and_number( );
        classname__filepath[chinese].push_back(*it_att_filePathAndName);
    }

    // 3.解析每个班的图片
    for (auto it_cfp = classname__filepath.begin( ); it_cfp != classname__filepath.end( ); it_cfp++) {
        myTable< chstring > sh;
        for (size_t i = 0; i < it_cfp->second.size( ); i++) {
            myTable< chstring > partSh;
            img::load_sheet_from_img(partSh, it_cfp->second[i]);
            sh = merge_table(sh, partSh);
            std::cout << U8C(u8"融合结束") << std::endl;
        }
        // 打印结果
        sheet_printer(sh);
        extract_attendance_to_vector(sh, it_cfp->first);
    }
    // 4.解析人员的签到情况到全人员表
    for (const auto &attper : att_person) {
        auto it_perstd = personStd_.end( );    // 哨兵值
        if (search_person(it_perstd, attper)) {
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
        chstring sheetTitle    = *it_className + U8C(u8"学员线下签到汇总");
        chstring sheetSavePath = chstring(file::_OUTPUT_ATT_DIR_) + *it_className + ".xlsx";

        myTable< chstring > sheet = {
            { U8C(u8"姓名"), U8C(u8"学号"),
              U8C(u8"学院"), U8C(u8"联系方式"),
              U8C(u8"签到"), U8C(u8"备注") }
        };

        for (auto it_person = personStd_.begin( ); it_person != personStd_.end( ); it_person++) {
            if (it_person->classname == *it_className) {
                myList< chstring > line;
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
        // sort_table_chstring_by(sheet, 4, false, true);    // 按照签到情况排序

        sort_table_chstring_by(
            sheet,
            false,
            true,
            [](const std::vector< chstring > &a, const std::vector< chstring > &b) -> bool {
                const size_t firstIndex  = 4;    // 第一排序索引
                const size_t secondIndex = 2;    // 第二排序索引
                if (firstIndex >= a.size( ) || firstIndex >= b.size( ) || secondIndex >= a.size( ) || secondIndex >= b.size( )) return false;
                if (a[firstIndex].u8string( ) < b[firstIndex].u8string( )) {
                    return true;
                } else if (a[firstIndex] |= b[firstIndex]) {
                    if (a[secondIndex].u8string( ) < b[secondIndex].u8string( ))
                        return true;
                    else
                        return false;
                } else {
                    return false;
                }
            });

        file::save_sttSheet_to_xlsx(sheet, sheetSavePath, sheetTitle);
    }
}

/* ======================================================================================================================= */
/* ======================================================================================================================= */

// @brief 青字班报名
void DoQingziClass::registration( ) {
    file::DefFolder *aFolder = new file::DefFolder(file::_INPUT_SIGN_QC_ALL_DIR_, true);
    if (!aFolder) {
        std::cout << U8C(u8"内存分配失败") << std::endl;
    }

    myList< std::string > paths = aFolder->get_sysfilepath_list(myList< chstring >{ ".docx", ".DOCX" });    // 文件路径
    // 解析docx文件
    if (paths.size( ) != 0) {
        for (const auto &p : paths) {
            docx::DefDocx aDocx(p);
            personStd_.push_back(aDocx.get_person( ));
        }
    }

    // 处理pdf
    file::DefFolder pdfFiles(*aFolder, myList< chstring >{ ".pdf", ".PDF" });
    paths = pdfFiles.get_u8filepath_list( );    // 文件路径
    // 解析pdf文件
    if (paths.size( ) != 0) {
        for (const auto &u8p : paths) {
            pdf::DefPdf aPdf(u8p);
            if (aPdf.isOKed( ) && aPdf.get_sheet_type( ) == pdf::DefPdf::SheetType::Committee) {    // 应聘表
                DefPerson per = aPdf.get_person( );
                auto      it  = personStd_.end( );
                search_person(it, per);
                if (it != personStd_.end( )) {
                    it->otherInformation[U8C(u8"文件地址")] += (u8p + " ; ");
                    it->ifsign       = true;
                    it->signPosition = per.signPosition;
                    if (!pdfFiles.erase_with(u8p))
                        pause( );
                } else {
                    per.ifsign                              = true;
                    per.otherInformation[U8C(u8"文件地址")] = u8p;
                    per.otherInformation[U8C(u8"报名方式")] = U8C(u8"组织推荐");
                    per.otherInformation[U8C(u8"备注")]     = U8C(u8"未找到docx文档");
                    personStd_.push_back(per);
                    if (!pdfFiles.erase_with(u8p))
                        pause( );
                }
                // std::cout << per.name;
                file::copy_file_to_folder(encoding::utf8_to_sysdcode(u8p), file::_OUTPUT_SIGN_QC_CMT_DIR_);    // 复制到输出文件夹
            } else if (aPdf.isOKed( ) && aPdf.get_sheet_type( ) == pdf::DefPdf::SheetType::Classmate) {
                DefPerson per = aPdf.get_person( );

                per.otherInformation[U8C(u8"文件地址")] = u8p;
                personStd_.push_back(per);
                pdfFiles.erase_with(u8p);
            }
        }
    }

    save_registrationSheet( );

    // 筛选pdf文件
    std::cout << std::endl
              << U8C(u8"复制了")
              << pdfFiles.copy_files_to(file::_OUTPUT_SIGN_QC_UNPDF_DIR_)
              << U8C(u8"份pdf文件到") << file::_OUTPUT_SIGN_QC_UNPDF_DIR_ << std::endl;

    std::cout << std::endl
              << U8C(u8"青字班报名表已输出到 ")
              << file::_OUTPUT_SIGN_QC_DIR_ << U8C(u8"报名.xlsx 中...")
              << std::endl;
    delete aFolder;
    aFolder = nullptr;    // 还是不要有野指针
}

// @brief 保存青字班的报名表
void DoQingziClass::save_registrationSheet( ) {
    myTable< chstring > sh{
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
        myList< chstring > line;
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
    // std::cout << "\n\njunmjkmknujk\n";

    sort_table_chstring_by(sh, 1);    // 自动排序
    deduplication_sheet(sh, myList< size_t >{ 0, 23 }, myList< size_t >{ 0 });
    // 修改第一列
    for (size_t i = 1; i < sh.size( ); i++)
        sh[i][0] = std::to_string(i);
    file::save_registrationSheet_to_xlsx(sh);
}


/*
 * @brief 搜索，从全人员名单中搜素目标人员信息
 * @param _it_output 总名单的一个迭代器
 * @param _targetPerson 目标的人员信息
 * @note 可以考虑怎么优化这两个search函数
 * @shit if很多吧，慢慢看  (^-^)
 */
bool DoQingziClass::search_person(myList< DefPerson >::iterator &_it_output, DefPerson _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if ((_targetPerson.classname |= it_all->classname)
                && (_targetPerson.name |= it_all->name)) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (compare_studentID(_targetPerson.studentID, it_all->studentID)) {
                        _it_output = it_all;
                        return true;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return true;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.name |= it_all->name) {
                if (_targetPerson.studentID.size( ) != 0) {
                    if (compare_studentID(_targetPerson.studentID, it_all->studentID)) {
                        _it_output = it_all;
                        return true;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return true;
                }
            } else {
                continue;
            }
        }
    }
    return false;
}

/*
 * @brief 搜索，从全人员名单中搜素目标人员信息
 * @param _it_output 总名单的一个迭代器
 * @param _targetPerson 目标的人员信息
 * @note 可以考虑怎么优化这两个search函数
 * @shit if很多吧，慢慢看  (^-^)
 */
bool DoQingziClass::search_person(myList< DefPerson >::iterator &_it_output, DefLine _targetPerson) {
    for (auto it_all = personStd_.begin( ); it_all != personStd_.end( ); it_all++) {
        /* 1.优先匹配班级（如果有） */
        if (_targetPerson.classname.size( ) != 0) {
            if ((_targetPerson.classname |= it_all->classname)
                && (_targetPerson.information[U8C(u8"姓名")] |= it_all->name)) {
                if (_targetPerson.information.find(U8C(u8"学号")) != _targetPerson.information.end( )) {
                    if (compare_studentID(_targetPerson.information[U8C(u8"学号")], it_all->studentID)) {
                        _it_output = it_all;
                        return true;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return true;
                }
            } else {
                continue;
            }
        } else {
            if (_targetPerson.information[U8C(u8"姓名")] |= it_all->name) {
                if (_targetPerson.information.find(U8C(u8"学号")) != _targetPerson.information.end( )) {
                    if (compare_studentID(_targetPerson.information[U8C(u8"学号")], it_all->studentID)) {
                        _it_output = it_all;
                        return true;
                    } else {
                        /*添加到疑似列表中*/
                        continue;
                    }
                } else {
                    // 没有学号？？？
                    _it_output = it_all;
                    return true;
                }
            } else {
                continue;
            }
        }
    }
    return false;
}

/*
 * @brief 比较学号
 * @return 相同返回true  不同返回false
 */
bool DoQingziClass::compare_studentID(const chstring &ia, const chstring &ib) {
    std::string a = ia.usstring( );
    std::string b = ib.usstring( );
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
void DoQingziClass::trans_personline_to_person(const DefLine &_inperLine, DefPerson &_outperStd) {
    DefPerson per;
    per.classname = _inperLine.classname;
    per.ifcheck   = _inperLine.ifcheck;
    per.ifsign    = _inperLine.ifcheck;
    for (auto it_inperLine = _inperLine.information.begin( );
         it_inperLine != _inperLine.information.end( );
         it_inperLine++) {
        if (it_inperLine->first |= U8C(u8"姓名")) {
            per.name = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"性别")) {
            per.gender = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"年级")) {
            per.grade = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"学号")) {
            per.studentID = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"政治面貌")) {
            per.politicaloutlook = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"学院")) {
            per.academy = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"专业")) {
            per.majors = it_inperLine->second;
        } else if ((it_inperLine->first |= U8C(u8"电话"))
                   || (it_inperLine->first |= U8C(u8"联系方式"))
                   || (it_inperLine->first |= U8C(u8"联系电话"))
                   || (it_inperLine->first |= U8C(u8"电话号码"))) {
            per.phonenumber = it_inperLine->second;
        } else if ((it_inperLine->first |= U8C(u8"QQ号"))
                   || (it_inperLine->first |= U8C(u8"qq号"))
                   || (it_inperLine->first |= U8C(u8"qq"))
                   || (it_inperLine->first |= U8C(u8"QQ"))) {
            per.qqnumber = it_inperLine->second;
        } else if ((it_inperLine->first |= U8C(u8"所任职务"))
                   || (it_inperLine->first |= U8C(u8"职务"))
                   || it_inperLine->first.has_subchstring(U8C(u8"职务"))
                   || it_inperLine->first.has_subchstring(U8C(u8"所任职务"))) {
            per.position = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"邮箱")) {
            per.email = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"民族")) {
            per.ethnicity = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"社团")) {
            if (it_inperLine->second.size( ) == 0) {
                per.club = U8C(u8"无");
            } else {
                per.club = it_inperLine->second;
            }
        } else if ((it_inperLine->first |= U8C(u8"报名青字班"))
                   || (it_inperLine->first |= U8C(u8"青字班"))
                   || (it_inperLine->first.has_subchstring(U8C(u8"青字班")))) {
            per.classname = it_inperLine->second;
        } else if (it_inperLine->first |= U8C(u8"应聘岗位")) {
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
void DoQingziClass::trans_person_to_personline(const DefPerson &_inperStd, DefLine &_outperLine) {
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
    if (_inperStd.otherInformation.size( ) != 0)
        for (auto it_in = _inperStd.otherInformation.begin( ); it_in != _inperStd.otherInformation.end( ); it_in++)
            per.information[it_in->first] = it_in->second;
    _outperLine = per;
}


// @brief 缓存全部报名的人员
void DoQingziClass::save_storageSheet( ) {
    // 按照 班级  姓名  学号  的方式保存
    // 仅保存报名的人员s
    myTable< chstring > sh;
    // 制表
    for (const auto &per : personStd_) {
        if (!per.ifsign) continue;
        myList< chstring > line;
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
    myTable< chstring > sh;
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
void DoQingziClass::save_unknown_person(const myList< DefUnknownPerson > &_in_unLists) {
    myTable< chstring > sh = {
        { "", U8C(u8"班级"), U8C(u8"姓名"),
          U8C(u8"学号"), U8C(u8"相似度") }
    };
    for (auto &unPer : _in_unLists) {
        myList< chstring > line1;
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
            myList< chstring > line2;
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

/*
 * @brief 打赢未知的人员
 * @param _unknownPersonList 未知人员信息表
 * @param _prompt 提示词
 */
void DoQingziClass::print_unknown_person(const myList< DefUnknownPerson > &_unknownPersonList, const std::string &_prompt) {
    std::cout << std::endl
              << std::endl
              << "\033[43;30mWARNING!!!\033[0m" << std::endl;
    std::cout << "\033[43;30m";
    std::cout << "###ATTENTION### ";
    std::cout << U8C(u8"图片中的以下人员不在全学员名单中");
    std::cout << " ###ATTENTION###";
    std::cout << std::endl;

    for (auto it_unknownPerson = _unknownPersonList.begin( );
         it_unknownPerson != _unknownPersonList.end( );
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
    if (!_prompt.empty( )) {
        std::cout << std::endl
                  << std::endl;
        std::cout << _prompt << std::endl;
    }
    std::cout << "\033[0m";
    std::cout << std::endl
              << std::endl;
}
