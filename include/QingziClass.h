#pragma once

#ifndef QINGZICLASS_H
#define QINGZICLASS_H

#include <basic.hpp>
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

    bool self_check( );    // 自检程序

    /*
     * @brief 一行信息转化为标准人员信息
     * @param _inperLine 一行信息
     * @param _outperStd 标准的人员信息
     */
    static void trans_personline_to_person(const DefLine &_inperLine, DefPerson &_outperStd);

    /*
     * @brief 标准人员信息转化为一行信息
     * @param _inperStd 标准的人员信息
     * @param _outperLine 一行信息
     * @note 这个函数好像没怎么用到
     */
    static void trans_person_to_personline(const DefPerson &_inperStd, DefLine &_outperLine);

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
    list< std::string > className_;          // 班级名字
    list< std::string > filePathAndName_;    // 每个标准xlsx文件的位置
    list< DefPerson >   personStd_;          // 定义的标准人员信息

    // 定义名单中不存在的人员
    struct DefUnknownPerson {
        DefLine           personLine;
        DefPerson         personStd;
        list< DefPerson > likelyPerson;    // 相似的人
        list< double >    likelyRate;      // 相似度，学号相同相似度直接达到100%,此时直接修改此人信息
    };

    list< DefUnknownPerson > unknownAppPerson_;    // 定义名单中不存在的(班委发过来的报名表)人员(直接对比)
    list< DefUnknownPerson > unknownAttPerson_;    // 名单中不存在（现场签到）的人员

    /* ========================================================================================= */

    /*
     * @brief 选择函数
     * @param _chosseAll 总选项数目
     * @param _outPrint 要打印在控制台上的内容
     */
    int choose_function(int _chosseAll, const list< std::string > &_outPrint);

    // @brief 加载全学员表的函数
    void load_all_personnel_information_list( );

    /* ========================================================================================= */

    // @brief 控制生成签到表的函数
    void applicants( );

    // @brief 统计报名人员
    void stats_applicants( );

    // @brief 保存签到表
    void save_applicantsSheet( );

    /* ========================================================================================= */

    // @brief 控制生成签到考勤表的函数
    void statistics( );

    //  @brief 统计签到人员
    void stats_checkinners( );

    // @brief 保存签到考勤表
    void save_statisticsSheet( );

    /* ========================================================================================= */

    // @brief 青字班报名
    void registration( );

    // @brief 保存青字班的报名表
    void save_registrationSheet( );

    /* ========================================================================================= */

    /*
     * @brief 搜索，从全人员名单中搜素目标人员信息
     * @param _it_output 总名单的一个迭代器
     * @param _targetPerson 目标的人员信息
     * @note 可以考虑怎么优化这四个search函数
     * @shit if很多吧，慢慢看  (^-^)
     */
    bool search_person(list< DefPerson >::iterator &_it_output, DefPerson _targetPerson);

    /*
     * @brief 搜索，从全人员名单中搜素目标人员信息
     * @param _it_output 总名单的一个迭代器
     * @param _targetPerson 目标的人员信息
     * @note 可以考虑怎么优化这四个search函数
     * @shit if很多吧，慢慢看  (^-^)
     */
    bool search_person(list< DefPerson >::iterator &_it_output, DefLine _targetPerson);

    // @brief 缓存全部报名的人员
    void save_storageSheet( );

    // @brief 加载缓存的全部报名的人员
    void load_storageSheet( );

    /*
     * @brief 保存尚未搜索到的成员
     * @param _in_unLists 未搜索到的成员列表
     */
    void save_unknown_person(const list< DefUnknownPerson > &_in_unLists);

    /*
     * @brief 打赢未知的人员
     * @param _unknownPersonList 未知人员信息表
     * @param _prompt 提示词
     */
    void print_unknown_person(const list< DefUnknownPerson > &_unknownPersonList, const std ::string &_prompt = "");
};



#endif    // !QINGZICLASS_H
