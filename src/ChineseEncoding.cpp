
#include "ChineseEncoding.h"
#include <consoleapi2.h>
#include <interlockedapi.h>    // PSLIST_HEADER
#include <iostream>
#include <string>
#include <string>
#include <stringapiset.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <winnt.h>    // CONTEXT, PCONTEXT

/*
 * @brief 检测 UTF - 8 合法性
 * @return false     不为UTF-8
 * @return true      是UTF-8
 */
bool is_valid_utf8(const std::string &str) {
    for (size_t i = 0; i < str.size( );) {
        unsigned char c   = str[i];
        int           len = 0;

        if ((c & 0x80) == 0)
            len = 1;    // ASCII
        else if ((c & 0xE0) == 0xC0)
            len = 2;    // 2字节
        else if ((c & 0xF0) == 0xE0)
            len = 3;    // 3字节（中文）
        else if ((c & 0xF8) == 0xF0)
            len = 4;    // 4字节
        else
            return false;    // 非法UTF-8

        for (int j = 1; j < len; ++j) {
            if (i + j >= str.size( )) return false;
            if ((str[i + j] & 0xC0) != 0x80) return false;
        }
        i += len;
    }
    return true;
}

/*
 * @brief 检测 GBK/GB2312
 * @return false     不为GBK/GB2312
 * @return true      是GBK/GB2312
 */
bool is_likely_gbk(const std::string &str) {
    for (size_t i = 0; i < str.size( );) {
        unsigned char c1 = str[i];
        if (c1 <= 0x7F) {
            i++;
            continue;
        }    // ASCII
        if (i + 1 >= str.size( )) return false;    // 不完整

        unsigned char c2 = str[i + 1];
        if (c1 >= 0x81 && c1 <= 0xFE && ((c2 >= 0x40 && c2 <= 0x7E) || (c2 >= 0x80 && c2 <= 0xFE))) {
            i += 2;
        } else {
            return false;
        }
    }
    return true;
}

/*
 * @brief 检测中文的编码
 * @return 枚举类型-编码方式
 */
ChineseEncoding detect_chinese_encoding(const std::string &str) {
    if (str.substr(0, 3) == "\xEF\xBB\xBF") return ChineseEncoding::UTF8_BOM;
    if (is_valid_utf8(str)) return ChineseEncoding::UTF8;
    if (is_likely_gbk(str)) return ChineseEncoding::GBKorGB2312;
    return ChineseEncoding::UNKNOWN;
}

/*
 * @brief 将gbk格式转化为utf8
 * @param gbk格式的string
 * @return 转化为utf8格式的string
 */
std::string gbk_to_utf8(const std::string &gbk_str) {
    if (gbk_str.empty( )) return "";

    // GBK → UTF-16
    int          wlen = MultiByteToWideChar(936, 0, gbk_str.c_str( ), -1, nullptr, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(936, 0, gbk_str.c_str( ), -1, &wstr[0], wlen);

    // UTF-16 → UTF-8
    int         len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str( ), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_str(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str( ), -1, &utf8_str[0], len, nullptr, nullptr);

    // 去掉结尾的 '\0'
    if (!utf8_str.empty( ) && utf8_str.back( ) == '\0') utf8_str.pop_back( );
    return utf8_str;
}

/*
 * @brief 将utf8格式转化为gbk
 * @param utf8格式的string
 * @return 转化为gbk格式的string
 */
std::string utf8_to_gbk(const std::string &utf8_str) {
    if (utf8_str.empty( )) return "";

    // UTF-8 → UTF-16
    int          wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str( ), -1, nullptr, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str( ), -1, &wstr[0], wlen);

    // UTF-16 → GBK
    int         len = WideCharToMultiByte(936, 0, wstr.c_str( ), -1, nullptr, 0, nullptr, nullptr);
    std::string gbk_str(len, 0);
    WideCharToMultiByte(936, 0, wstr.c_str( ), -1, &gbk_str[0], len, nullptr, nullptr);

    if (!gbk_str.empty( ) && gbk_str.back( ) == '\0') gbk_str.pop_back( );
    return gbk_str;
}

/*
 * @brief 将任意中文（简体）格式转化为utf8
 * @param 任意中文（简体）格式的string
 * @return 转化为utf8格式的string
 */
std::string anycode_to_utf8(const std::string &anycode) {
    ChineseEncoding chcode = detect_chinese_encoding(anycode);

    switch (chcode) {
        case ChineseEncoding::UNKNOWN:
            /*错误检测*/
            return "";
            break;
        case ChineseEncoding::UTF8_BOM:
            return anycode;
            break;
        case ChineseEncoding::GBKorGB2312:
            return gbk_to_utf8(anycode);
            break;
        case ChineseEncoding::UTF8:
            return anycode;
            break;
        default:
            // 没有default
            break;
    }
}
