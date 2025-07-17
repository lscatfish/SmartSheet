#pragma once

#ifndef QINGZICLASS_H
#define QINGZICLASS_H

#include "ChineseEncoding.h"
#include "Files.h"
#include "PersonnelInformation.h"
#include <string>
#include <vector>


// 操作青字班的类
class DoQingziClass {
public:
    DoQingziClass( );
    ~DoQingziClass( );
    void start( );    // 主控函数

    // 人员信息的记录方式（标准与非标准）
    enum class PersonFormat {
        UNSTD = 0,    // 非标准的人员信息
        STD           // 标准的人员信息
    };

private:
    PersonFormat perInFormat_;    // 人员信息的记录方式

    std::vector< std::string >    className_;          // 班级名字
    std::vector< std::string >    filePathAndName_;    // 每个xlsx文件的位置
    std::vector< DefStdPerson >   personStd_;          // 定义的标准人员信息
    std::vector< DefUnstdPerson > personUnstd_;        // 定义的非标准人员信息
    /*
     * @brief 加载全学员表的函数
     */
    void load_personnel_information_list( );

    /*
     * @brief 加载报名表
     */
    void load_applicationSheet_for_attendanceSheet( );
};



#endif    // !QINGZICLASS_H
