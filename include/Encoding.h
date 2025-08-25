#pragma once

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
std::wstring utf8_to_wstring_win(const std::string u8);

/*
 * @brief 将任意中文（简体）格式转化为utf8
 * @param _anycode 任意中文（简体）格式的string
 * @return 转化为utf8格式的string
 */
std::string sysdcode_to_utf8(const std::string &_anycode);

/*
 * @brief 检测字符串的编码格式是否为utf8
 * @param _u8 待检测的字符串
 * @return 是utf8返回true，否则返回false
 */
bool is_utf8(const std::string &_u8);

}    // namespace encoding

#endif    // !CHINESEENCODING_H
