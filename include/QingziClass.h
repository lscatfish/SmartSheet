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
    std::vector< DefUnstdPerson > errorPerson_;        // 定义名单中不存在的人员
    /*
     * @brief 加载全学员表的函数
     */
    void load_personnel_information_list( );

    /*
     * @brief 制作签到表
     */
    void make_attendanceSheet( );

    /*
     * @brief 制作考勤统计表
     */
    void make_statisticsSheet( );

    /*
    * @brief 保存签到表
    */
    void save_attendanceSheet( );

    /*
     * @brief 搜索，从全人员名单中搜素目标人员信息
     * @param 总名单的一个迭代器
     * @param 目标的人员信息
     * @note 可以考虑怎么优化这四个search函数
     * @shit if很多吧，慢慢看  (^_^)
     */
    void search_person(
        std::vector< DefStdPerson >::iterator &_it_output,
        DefStdPerson                           _targetPerson);

    /*
     * @brief 搜索，从全人员名单中搜素目标人员信息
     * @param 总名单的一个迭代器
     * @param 目标的人员信息
     * @note 可以考虑怎么优化这四个search函数
     * @shit if很多吧，慢慢看  (^_^)
     */
    void search_person(
        std::vector< DefStdPerson >::iterator &_it_output,
        DefUnstdPerson                         _targetPerson);

    /*
     * @brief 搜索，从全人员名单中搜素目标人员信息
     * @param 总名单的一个迭代器
     * @param 目标的人员信息
     * @note 可以考虑怎么优化这四个search函数
     * @shit if很多吧，慢慢看  (^_^)
     */
    void search_person(
        std::vector< DefUnstdPerson >::iterator &_it_output,
        DefStdPerson                             _targetPerson);

    /*
     * @brief 搜索，从全人员名单中搜素目标人员信息
     * @param 总名单的一个迭代器
     * @param 目标的人员信息
     * @note 可以考虑怎么优化这四个search函数
     * @shit if很多吧，慢慢看  (^_^)
     */
    void search_person(
        std::vector< DefUnstdPerson >::iterator &_it_output,
        DefUnstdPerson                           _targetPerson);
};



#endif    // !QINGZICLASS_H
