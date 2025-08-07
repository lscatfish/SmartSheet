#pragma once

#ifndef PERSONNELINFORMATION_H
#define PERSONNELINFORMATION_H

/*
 * 此文件用于定义人员信息
 */

#include <map>
#include <string>

/*
 * 此结构体用于定义标准人员信息
 */
struct DefStdPerson {
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
    bool        ifcheck;             // 是否签到
    bool        ifsign;              // 是否报名

    std::map< std::string, std::string > otherInformation;    // 其他信息


    // @brief 基本构造函数
    DefStdPerson( ) {
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
    DefStdPerson(
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
        ifcheck     = false;
        ifsign      = false;
    };

    /*
     * @brief 析构函数
     */
    ~DefStdPerson( ) {};
};

/*
 * 此结构体用于定义非标准的人员信息
 */
struct DefUnstdPerson {
public:
    std::string                          classname;    // 班级名字
    std::map< std::string, std::string > information;
    bool                                 ifcheck;    // 是否签到
    bool                                 ifsign;     // 是否报名

    // @brief 基本构造函数
    DefUnstdPerson( ) {
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
    DefUnstdPerson(
        std::string _classname,
        std::string _name,
        std::string _studentID,
        std::string _academy,
        std::string _phonenumber) {
        ifcheck                                  = false;
        ifsign                                   = false;
        classname                                = _classname;
        information[anycode_to_utf8("姓名")]     = anycode_to_utf8(_name);
        information[anycode_to_utf8("学号")]     = anycode_to_utf8(_studentID);
        information[anycode_to_utf8("学院")]     = anycode_to_utf8(_academy);
        information[anycode_to_utf8("联系电话")] = anycode_to_utf8(_phonenumber);
    };

    /*
     * @brief 析构函数
     */
    ~DefUnstdPerson( ) {};
};

#endif    // !PERSONNELINFORMATION_H
