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

// 清理缓冲区
void clear_input_buffer( );

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

// 清除字符串前后的所有空白字符（包括空格、\t、\n等）
std::string trim_whitespace(const std::string &str);

// 分割字符串，获取'='前后的内容
// 返回值: pair的first是'='前的字符串，second是'='后的字符串
// 如果没有找到'='，则first为原字符串，second为空
std::pair< std::string, std::string > split_by_equal(const std::string &str);

// 清空控制台
void clearConsole( );

/**
 * 设置控制台输入输出编码为UTF-8
 * 支持Windows和Linux/macOS系统
 */
void set_console_utf8( );

// 开始前警告
bool start_warning( );


#endif    // !HELPER_H