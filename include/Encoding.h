#pragma once

#ifndef ENCODING_H
#define ENCODING_H

#include <string>

namespace encoding    // 使用时请用namespace encoding=ec;
{

///+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 枚举文字编码
enum class ENCODING {
    UNKNOWN = 0,    // 未知编码（错误）
    // Unicode 相关编码
    UTF8,       // utf8
    UTF16BE,    // UTF-16 大端序（无 BOM）
    UTF16LE,    // UTF-16 小端序（无 BOM）
    UTF32BE,    // UTF-32 大端序
    UTF32LE,    // UTF-32 小端序
    // 中文相关编码
    GBK,        // 扩展的中文编码（包含 GB2312 及更多字符）
    GBK2312,    // 基础中文编码（简体字）
    GB18030,    // 更全面的中文编码（兼容 GBK，支持少数民族文字）
    Big5,       // 繁体中文编码（主要用于台湾、香港）
    // 拉丁语系及西欧编码
    ISO_8859_1,      //	西欧语言（英语、法语、德语等）
    ISO_8859_2,      // 中欧及东欧语言（波兰语、捷克语等）
    ISO_8859_5,      // 西里尔字母（俄语、保加利亚语等）
    ISO_8859_7,      // 希腊语，部分中文会检测到希腊语
    windows_1252,    // 微软扩展的西欧编码（兼容 ISO-8859-1）
    ASCII,           // 纯 ASCII编码（0x00 - 0x7F 范围内字符）
    // 其他常见编码
    KOI8_R,       // 俄语编码
    Shift_JIS,    // 日语
    EUC_JP,       // 日语拓展
    EUC_KR,       // 韩语
    ISO_8859_9    // 土耳其语
};
///+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


/*
 * @brief 使用uchardet检测string内容
 * @param str 输入
 * @return 编码值
 */
ENCODING detect_encoding(const std::string &str);

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
 * @brief utf8转wstring
 * @param utf8编码的string
 * @return 宽字符wstring（win里面实际是utf-16）
 * @note 只能在windows编译
 **/
std::wstring utf8_to_wstring_win(const std::string u8);

/*
 * @brief 将任意中文（简体）格式转化为utf8
 * @param _anycode 任意中文（简体）格式的string
 * @return 转化为utf8格式的string
 */
std::string chcode_to_utf8(const std::string &_anycode);


}    // namespace encoding

#endif    // !CHINESEENCODING_H
