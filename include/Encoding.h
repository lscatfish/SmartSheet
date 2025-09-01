#pragma once

/* ====================================================================================================== *
 *
 * 此文件用于处理各种编码问题
 *
 * 作者：lscatfish
 *
 * ======================================================================================================= */

#ifndef ENCODING_H
#define ENCODING_H

#include <icu_encoding_handler.h>
#include <string>


namespace encoding    // 使用时请用namespace encoding=ec;
{
// 系统默认的编码方式
extern std::string systemDefaultEncoding;

// 启动函数，配置编码
void Init( );

/*
 * @brief utf8转wstring
 * @param u8 utf8编码的string
 * @return 宽字符wstring（win里面实际是utf-16）
 * @note 只能在windows编译
 **/
std::wstring utf8_to_wstring(const std::string u8);

/*
 * @brief 将任意中文（简体）格式转化为utf8
 * @param _anycode 任意中文（简体）格式的string
 * @return 转化为utf8格式的string
 */
std::string sysdcode_to_utf8(const std::string &_anycode);

/*
 * @brief 将utf8转化为系统默认的中文（简体）编码
 * @param _anycode 任意u8格式的string
 * @return 转化为系统格式格式的string
 */
std::string utf8_to_sysdcode(const std::string &_u8);

/*
 * @brief 检测字符串的编码格式是否为utf8
 * @param _u8 待检测的字符串
 * @return 是utf8返回true，否则返回false
 */
bool is_utf8(const std::string &_u8);

/*
 * @brief 检测utf8字符串是否有错误
 * @param utf8 要检测的utf8字符串
 * @return 返回是否有效
 */
bool check_utf8_validity(const std::string &utf8);

/*
 * @brief 检测一个table<string>内部是否有非法字符集
 * @param _sheet 输入的sheet
 * @return 返回是否有效
 */
bool check_sheet_utf8_validity(const table< std::string > &_sheet);

/*
 * @brief 修复table<string>内部的非法字符集错误
 * @param _sheet 输入的sheet
 * @return 返回修复的字符集的错误数量
 */
size_t repair_sheet_utf8_invalidity(table< std::string > &_sheet);

}    // namespace encoding

#endif    // !CHINESEENCODING_H
