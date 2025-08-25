#pragma once
#include <basic.hpp>
#include <string>
#include <utility>
#include <vector>

/*
 * @file helper.h
 * @brief 一些辅助功能的函数
 */

#ifndef HELPER_H
#define HELPER_H

/*
 * @brief 按回车键继续
 */
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

// 清除字符串前面的所有空格
std::string trim_whitespace(const std::string &str);

// 清空控制台
void clearConsole( );


#endif    // !HELPER_H