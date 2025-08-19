#pragma once

#ifndef PERSONNELINFORMATION_H
#define PERSONNELINFORMATION_H

/*
 * 此文件用于定义人员信息
 */

#include <Encoding.h>
#include <map>
#include <string>

/*
 * 此结构体用于定义标准人员信息
 */
struct DefPerson {
public:
    std::string classname;           // 班级名字
    std::string name;                // 姓名
    std::string gender;              // 性别
    std::string grade;               // 年级
    std::string studentID;           // 学号
    std::string politicaloutlook;    // 政治面貌
    std::string academy;             // 学院
    std::string majors;              // 专业
    std::string phonenumber;         // 电话
    std::string qqnumber;            // QQ号

    std::string position;            // 所任职务
    std::string email;               // 邮箱
    std::string ethnicity;           // 民族
    std::string club;                // 社团

    bool        ifcheck;             // 是否签到
    bool        ifsign;              // 是否报名

    std::map< std::string, std::string > otherInformation;    // 其他信息


    // @brief 基本构造函数
    DefPerson( ) {
        classname.erase( );
        name.erase( );
        gender.erase( );
        grade.erase( );
        studentID.erase( );
        politicaloutlook.erase( );
        academy.erase( );
        majors.erase( );
        phonenumber.erase( );
        qqnumber.erase( );
        position.erase( );
        email.erase( );
        ethnicity.erase( );
        club.erase( );
        ifcheck = false;
        ifsign  = false;
    };

    /*
     * @brief 签到表的构造函数
     * @param 青字班班级名称
     * @param 名字
     * @param 学号
     * @param 学院
     * @param 电话
     */
    DefPerson(
        std::string _classname,
        std::string _name,
        std::string _studentID,
        std::string _academy,
        std::string _phonenumber) {
        classname.erase( );
        name.erase( );
        gender.erase( );
        grade.erase( );
        studentID.erase( );
        politicaloutlook.erase( );
        academy.erase( );
        majors.erase( );
        phonenumber.erase( );
        qqnumber.erase( );
        classname   = _classname;
        name        = _name;
        studentID   = _studentID;
        academy     = _academy;
        phonenumber = _phonenumber;
        position.erase( );
        email.erase( );
        ethnicity.erase( );
        club.erase( );
        ifcheck     = false;
        ifsign      = false;
    };

    /*
     * @brief 析构函数
     */
    ~DefPerson( ) {};
};

/*
 * 此结构体用于定义非标准的人员信息(sheet中的一行)
 */
struct DefLine {
public:
    std::string                          classname;    // 班级名字
    std::map< std::string, std::string > information;
    bool                                 ifcheck;    // 是否签到
    bool                                 ifsign;     // 是否报名

    // @brief 基本构造函数
    DefLine( ) {
        classname.erase( );
        ifcheck = false;
        ifsign  = false;
    };

    /*
     * @brief 签到表的构造函数
     * @param 青字班班级名称
     * @param 名字
     * @param 学号
     * @param 学院
     * @param 电话
     */
    DefLine(
        std::string _classname,
        std::string _name,
        std::string _studentID,
        std::string _academy,
        std::string _phonenumber) {
        ifcheck                   = false;
        ifsign                    = false;
        classname                 = _classname;
        information[u8"姓名"]     = encoding::sysdcode_to_utf8(_name);
        information[u8"学号"]     = encoding::sysdcode_to_utf8(_studentID);
        information[u8"学院"]     = encoding::sysdcode_to_utf8(_academy);
        information[u8"联系电话"] = encoding::sysdcode_to_utf8(_phonenumber);
    };

    /*
     * @brief 析构函数
     */
    ~DefLine( ) {};
};

#endif    // !PERSONNELINFORMATION_H
