/* ===================================================================
 *				================
 *				||模糊算法模块||
 *				================
 * @brief 此文件用于操作各种模糊函数
 * @brief 模糊函数包含模糊搜索、模糊匹配等函数，支持C++标准string
 * @note  模糊搜索函数都是由 2561925435@qq.com 独自编写，若出现错误，还请修改
 * (+^-^+)
 *
 * 作者：   刘思成
 * 邮箱：   2561925435@qq.com
 * ----------------------------------------------------------------- */

#pragma once
#ifndef FUZZY_H
#define FUZZY_H

#include <cstdlib>
#include <iostream>
#include <PersonnelInformation.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace fuzzy {
/*
 * 匹配度
 */
enum class LEVEL {
    High = 0,    //   1.0 - 0.9    Levenshtein  0
    Medium,      //   0.9 - 0.7    Levenshtein  0-1
    Low,         //   0.7 - 0.5    Levenshtein  0-2
    NONE         //   (^-^)        离谱
};

/*
 * @brief 模糊搜索函数
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param 搜索目标
 * @param 索引库
 * @param 匹配度
 * @return 系列可能的答案
 */
std::vector< std::string > search(
    std::string                       _target,
    const std::vector< std::string > &_searchingLib,
    LEVEL                             _matchLevel);

/*
 * @brief 模糊搜索函数
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param 系列可能的答案
 * @param 搜索目标
 * @param 索引库
 * @param 匹配度
 * @return 是否搜索成功
 */
bool search(
    std::vector< std::string >       &_outList,
    std::string                       _target,
    const std::vector< std::string > &_searchingLib,
    LEVEL                             _matchLevel);

/*
 * @brief 模糊搜索函数,没有返回可能匹配的答案
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param _searchingLib 索引库
 * @param _target 搜索目标
 * @param _matchLevel 匹配度
 * @return 是否搜索成功
 */
bool search(
    const std::vector< std::string > &_searchingLib,
    std::string                       _target,
    LEVEL                             _matchLevel);

/*
 * @brief 模糊搜索人员信息
 * @param _outList 系列可能的人员信息
 * @param _likelyRate 相似度
 * @param _target 搜索目标
 * @param _searchingLib 搜索库
 * @retrun 是否搜索成功
 */
bool search_for_person(
    std::vector< DefPerson >       &_outList,
    std::vector< double >          &_likelyRate,
    DefPerson                       _target,
    const std::vector< DefPerson > &_searchingLib);

}    // namespace fuzzy



#endif    // !FUZZY_H
