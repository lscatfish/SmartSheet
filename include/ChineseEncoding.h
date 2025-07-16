#pragma once

#ifndef CHINESEENCODING_H
#define CHINESEENCODING_H

#include <string>

///+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 枚举中文编码
enum class ChineseEncoding {
    UNKNOWN = 0,    // 未知编码（错误）
    UTF8_BOM,       // utf8带签名
    GBKorGB2312,    // 国标中文编码
    UTF8            // utf8
};
///+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


/*
 * @brief 检测 UTF - 8 合法性
 * @return false     不为UTF-8
 * @return true      是UTF-8
 */
bool is_valid_utf8(const std::string &str);

/*
 * @brief 检测 GBK/GB2312
 * @return false     不为GBK/GB2312
 * @return true      是GBK/GB2312
 */
bool is_likely_gbk(const std::string &str);

/*
 * @brief 检测中文的编码
 * @return 枚举类型-编码方式
 */
ChineseEncoding detect_chinese_encoding(const std::string &str);

/*
 * @brief 将gbk格式转化为utf8
 * @param gbk格式的string
 * @return 转化为utf8格式的string
 */
std::string gbk_to_utf8(const std::string &gbk_str);

/*
 * @brief 将utf8格式转化为gbk
 * @param utf8格式的string
 * @return 转化为gbk格式的string
 */
std::string utf8_to_gbk(const std::string &utf8_str);

/*
 * @brief 将任意中文（简体）格式转化为utf8
 * @param 任意中文（简体）格式的string
 * @return 转化为utf8格式的string
 */
std::string anycode_to_utf8(const std::string &anycode);



#endif    // !CHINESEENCODING_H
