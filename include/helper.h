#pragma once

/*
 * @file helper.h
 * @brief 一些辅助功能的函数
 * 里面的函数功能很杂
 *
 * 作者：lscatfish
 * 邮箱：2561925435@qq.com
 */

#ifndef HELPER_H
#define HELPER_H

#include <basic.hpp>
#include <string>
#include <utility>
#include <vector>


// @brief 按回车键继续
void pause( );

// 分离字符串，返回一个 pair，第一个元素是中文部分，第二个元素是数字部分
std::pair< std::string, std::string > split_chinese_and_number(const std::string &input);

// 递归终止函数
void mergeHelper(table< std::string > &result);

// 可变参数模板函数，用于合并任意数量的vector<vector<string>>
template < typename... Args >
void mergeHelper(table< std::string >       &result,
                 const table< std::string > &first,
                 Args &&...rest) {
    // 添加当前vector的所有元素
    result.insert(result.end( ), first.begin( ), first.end( ));
    // 递归处理剩余的vector
    mergeHelper(result, std::forward< Args >(rest)...);
}

// 合并任意数量的vector<vector<string>>
template < typename... Args >
table< std::string > mergeMultipleSheets(Args &&...args) {
    table< std::string > result;
    mergeHelper(result, std::forward< Args >(args)...);
    return result;
}

// 合并两个list<std::string>
list< std::string > merge_two_string_lists(const list< std::string > &list1, const list< std::string > &list2);

// 清除字符串前后的所有空白字符（包括空格、\t、\n等）
std::string trim_whitespace(const std::string &str);

// 分割字符串，获取'='前后的内容
// 返回值: pair的first是'='前的字符串，second是'='后的字符串
// 如果没有找到'='，则first为原字符串，second为空
std::pair< std::string, std::string > split_by_equal(const std::string &str);

/*
 * @brief 分割字符串，获取第一目标字符的前后的内容
 * @param str 输入的字符串
 * @param cutter 分割的字符
 */
std::pair< std::string, std::string > split_by_first_of(const std::string &str, const char cutter);

/*
 * @brief 分割字符串，获取最后目标一个字符的前后的内容
 * @param str 输入的字符串
 * @param cutter 分割的字符
 */
std::pair< std::string, std::string > split_by_last_of(const std::string &str, const char cutter);

/*
 * @brief 分割字符串，获取第一目标字符的前后的内容
 * @param str 输入的字符串
 * @param cutter 分割的字符
 */
std::pair< std::string, std::string > split_by_first_of(const std::string &str, const std::string &cutter);

/*
 * @brief 分割字符串，获取最后目标一个字符的前后的内容
 * @param str 输入的字符串
 * @param cutter 分割的字符
 */
std::pair< std::string, std::string > split_by_last_of(const std::string &str, const std::string &cutter);

// 判断一个字符串是否都是数字
bool is_all_digits(const std::string &s);

/*
 * @brief 对table< string > 进行排序
 * @param _inTable 待排序的表格
 * @param _sortColIndex 依据哪一列进行排序（从0开始计数）
 * @param _ascending 是否升序排序，默认为true（升序）
 * @param _keepFirstCol 是否保持第一列不变，默认为true
 * @param _excludeHeader 是否排除表头（第一行）进行排序，默认为true
 * @return 排序是否成功
 */
bool sort_table_string_by(
    table< std::string > &_inTable,
    size_t                _sortColIndex,
    bool                  _ascending     = true,
    bool                  _keepFirstCol  = true,
    bool                  _excludeHeader = true);

/*
 * @brief 对table< string > 进行排序(自定义排序)
 * @param _inTable 待排序的表格
 * @param _keepFirstCol 是否保持第一列不变
 * @param _excludeHeader 是否排除表头（第一行）进行排序
 * @param comparator 自定义的排序函数
 * @return 排序是否成功
 */
template < typename Compare >
bool sort_table_string_by(
    table< std::string > &_inTable,
    bool                  _keepFirstCol,
    bool                  _excludeHeader,
    Compare               comparator) {

    if (_inTable.size( ) <= 1) return false;    // 如果表格行数小于等于1，无需排序
    // 交换两行
    auto swap_rows = [_keepFirstCol](std::vector< std::string > &a, std::vector< std::string > &b) {
        std::swap(a, b);
        if (_keepFirstCol && a.size( ) > 0 && b.size( ) > 0) {
            std::swap(a[0], b[0]);    // 保持第一列不变
        }
    };
    size_t startRow = _excludeHeader ? 1 : 0;    // 排除表头则从第二行开始排序
    // 冒泡排序
    for (size_t i = startRow; i < _inTable.size( ); i++) {
        for (size_t j = startRow + 1; j < _inTable.size( ); j++) {
            if (comparator(_inTable[j - 1], _inTable[j])) {
                swap_rows(_inTable[j - 1], _inTable[j]);
                // std::cout << "c";
            }
        }
    }
    return true;
}

// 检查字符串是否仅包含英文字母和数字
bool is_alpha_numeric(const std::string &str);

// 将字符串中的大写字母转为小写，数字保持不变
std::string lower_alpha_numeric(const std::string &str);

/*
 * @brief 去重：去除table中的重复行
 * @param _sh 一个sheet
 * @param _excludeCols 排除的列
 * @param _excludeRows 排除的行
 */
void deduplication_sheet(table< std::string > &_sh, const list< size_t > &_excludeCols, const list< size_t > &_excludeRows);

#endif    // !HELPER_H
