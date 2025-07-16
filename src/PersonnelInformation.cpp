
#include "ChineseEncoding.h"
#include "PersonnelInformation.h"

// @brief 基本构造函数
DefStdPerson::DefStdPerson( ) {
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
DefStdPerson::DefStdPerson(
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
DefStdPerson::~DefStdPerson( ) {}


// @brief 基本构造函数
DefUnstdPerson::DefUnstdPerson( ) {
    classname.erase( );
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
DefUnstdPerson::DefUnstdPerson(
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
}

/*
 * @brief 析构函数
 */
DefUnstdPerson::~DefUnstdPerson( ) {}