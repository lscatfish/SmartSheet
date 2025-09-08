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

#include <basic.hpp>
#include <chstring.hpp>
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
    NONE,        //   (^-^)        离谱
    Part,        //   部分匹配    子串匹配
};

/*
 * @brief 一般搜索函数，用于其他的搜索，一定要完全匹配才行
 * @param _target 搜索目标
 * @param _searchingLib 索引库
 * @return 是否搜索成功
 */
template < typename _SearchType >
bool perfect_match(const _SearchType &_target, const myList< _SearchType > &_searchingLib) {
    for (const auto &s : _searchingLib) {
        if (s == _target) return true;
    }
    return false;
}

/*
 * @brief 一般搜索函数，用于其他的搜索，一定要完全匹配才行
 * @param _target 搜索目标
 * @param _searchingLib 索引库
 * @return 搜索到的list
 */
template < typename _SearchType >
myList< _SearchType > search_perfect_match(const _SearchType &_target, const myList< _SearchType > &_searchingLib) {
    myList< _SearchType > out;
    for (const auto &s : _searchingLib) {
        if (s == _target) {
            out.push_back(s);
        }
    }
    return out;
}

/*
 * @brief 模糊搜索函数
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param _target 搜索目标
 * @param _searchingLib 索引库
 * @param _matchLevel 匹配度
 * @return 系列可能的答案
 */
myList< chstring > search(
    const chstring          _target,
    const myList< chstring > &_searchingLib,
    const LEVEL             _matchLevel);

/*
 * @brief 模糊搜索函数
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param _outList 系列可能的答案
 * @param _target 搜索目标
 * @param _searchingLib 索引库
 * @param _matchLevel 匹配度
 * @return 是否搜索成功
 */
bool search(
    myList< chstring >       &_outList,
    const chstring          _target,
    const myList< chstring > &_searchingLib,
    const LEVEL             _matchLevel);

/*
 * @brief 模糊搜索函数,没有返回可能匹配的答案
 * @note 用于支持部分文字编码、数字编码的模糊搜索
 * @param _searchingLib 索引库
 * @param _target 搜索目标
 * @param _matchLevel 匹配度
 * @return 是否搜索成功
 */
bool search(
    const myList< chstring > &_searchingLib,
    const chstring          _target,
    const LEVEL             _matchLevel);

/*
 * @brief 模糊搜索人员信息
 * @param _outList 系列可能的人员信息
 * @param _likelyRate 相似度
 * @param _target 搜索目标
 * @param _searchingLib 搜索库
 * @retrun 是否搜索成功
 */
bool search_for_person(
    myList< DefPerson >       &_outList,
    myList< double >          &_likelyRate,
    DefPerson                _target,
    const myList< DefPerson > &_searchingLib);

/*
 * @brief 检查主字符串中是否包含子串
 * @param _mainStr 主字符串
 * @param _subStr 子字符串
 */
bool search_substring(const std::string &_mainStr, const std::string &_subStr);

}    // namespace fuzzy



#endif    // !FUZZY_H
