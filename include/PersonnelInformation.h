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

    // @brief 基本构造函数
    DefStdPerson( );

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
        std::string _phonenumber);

    /*
     * @brief 析构函数
     */
    ~DefStdPerson( );
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
    DefUnstdPerson( );

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
        std::string _phonenumber);

    /*
     * @brief 析构函数
     */
    ~DefUnstdPerson( );
};

#endif    // !PERSONNELINFORMATION_H
