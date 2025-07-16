
#include "PersonnelInformation.h"

// @brief 基本构造函数
DefPerson::DefPerson( ) {
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
}

/*
 * @brief 签到表的构造函数
 * @param 青字班班级名称
 * @param 名字
 * @param 学号
 * @param 学院
 * @param 电话
 */
DefPerson::DefPerson(
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
}

/*
 * @brief 析构函数
 */
DefPerson::~DefPerson( ) {}