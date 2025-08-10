#pragma once

#ifndef QINGZICLASS_H
#define QINGZICLASS_H

#include <ChineseEncoding.h>
#include <Files.h>
#include <map>
#include <PersonnelInformation.h>
#include <string>
#include <vector>


// 操作青字班的类
class DoQingziClass {
public:
    DoQingziClass( );
    ~DoQingziClass( );
    void start( );    // 主控函数

    /*
     * @brief 一行信息转化为标准人员信息
     * @param 一行信息
     * @param 标准的人员信息
     */
    static void trans_line_to_person(const DefLine &_inperLine, DefPerson &_outperStd);

    /*
     * @brief 标准人员信息转化为一行信息
     * @param 标准的人员信息
     * @param 一行信息
     */
    static void trans_person_to_line(const DefPerson &_inperStd, DefLine _outperLine);

    /*
     * @brief 比较学号
     * @return 相同返回true  不同返回false
     */
    static bool compare_studentID(const std::string &a, const std::string &b);

    // 人员信息的记录方式（标准与非标准）
    enum class PersonFormat {
        UNSTD = 0,    // 非标准的人员信息
        STD           // 标准的人员信息
    };

private:
    // PersonFormat perInFormat_;    // 人员信息的记录方式

    std::vector< std::string > className_;          // 班级名字
    std::vector< std::string > filePathAndName_;    // 每个标准xlsx文件的位置
    std::vector< DefPerson >   personStd_;          // 定义的标准人员信息
    // std::vector< DefLine > personUnstd_;        // 定义的非标准人员信息
    //定义名单中不存在的人员
    struct DefUnknownPerson {
        DefLine personLine;
        DefPerson personStd;
        std::vector< DefPerson > likelyPerson;//相似的人
    };

    std::vector< DefUnknownPerson > unknownPerson_;    // 定义名单中不存在的人员(直接对比)

    // 不存在人员的模糊搜寻结果
   // std::map< std::vector< DefLine >::iterator, std::vector< DefLine > > fuzzyPerson_;

    /*
     * @brief 加载全学员表的函数
     */
    void
    load_personnel_information_list( );

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
        std::vector< DefPerson >::iterator &_it_output,
        DefPerson                           _targetPerson);

    /*
     * @brief 搜索，从全人员名单中搜素目标人员信息
     * @param 总名单的一个迭代器
     * @param 目标的人员信息
     * @note 可以考虑怎么优化这四个search函数
     * @shit if很多吧，慢慢看  (^_^)
     */
    void search_person(
        std::vector< DefPerson >::iterator &_it_output,
        DefLine                             _targetPerson);
};



#endif    // !QINGZICLASS_H
