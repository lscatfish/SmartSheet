
#include "ChineseEncoding.h"
#include "Files.h"
#include "QingziClass.h"
#include <iostream>
#include <PersonnelInformation.h>
#include <string>
#include <vector>

DoQingziClass::DoQingziClass( ) {
}

DoQingziClass::~DoQingziClass( ) {
}

/* @brief 主控函数 */
void DoQingziClass::start( ) {
}

/*
 * @brief 加载全学员表的函数
 * @param 人员记录方式
 */
void DoQingziClass::load_personnel_information_list(PersonFormat in_format) {
    // lambda函数定义

    // 保存标准的人员信息
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

    // 保存非标准的人员信息
    auto save_information_unstd =
        [](const std::vector< std::vector< std::string > > &sh, std::string cn) -> void {

    };

    get_filepath_from_folder(className_, filePathAndName_, anycode_to_utf8("./input/all/"));


    // 按文件读取每个青字班的信息表
    for (
        auto it_className = className_.begin( ), it_filePathAndName = filePathAndName_.begin( );
        it_className != className_.end( ) && it_filePathAndName != filePathAndName_.end( );
        it_className++, it_filePathAndName++) {
        // 保存读取到的表格
        std::vector< std::vector< std::string > > sheet;
        load_sheet_from_file(sheet, *it_filePathAndName);
        if (in_format == PersonFormat::STD) {
            save_information_std(sheet, *it_className);
        } else if (in_format == PersonFormat::UNSTD) {
            save_information_unstd(sheet, *it_className);
        }
    }
}
