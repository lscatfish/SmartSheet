
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
        unsigned char c   = static_cast< unsigned char >(str[i]);
        int           len = 0;

        if ((c & 0x80) == 0) {
            len = 1;    // ASCII (0x00-0x7F)
        } else if ((c & 0xE0) == 0xC0) {
            len = 2;    // 双字节 (0xC0-0xDF)
        } else if ((c & 0xF0) == 0xE0) {
            len = 3;    // 三字节 (0xE0-0xEF)
        } else if ((c & 0xF8) == 0xF0) {
            // 四字节：必须在 0xF0-0xF4 范围内（避免超 Unicode 范围）
            if (c > 0xF4) return false;
            len = 4;
        } else {
            return false;    // 非法首字节（如 0xF8-0xFF）
        }

        // 检查后续字节是否越界，且均以 0x80 开头
        for (int j = 1; j < len; ++j) {
            if (i + j >= str.size( )) return false;
            unsigned char next_c = static_cast< unsigned char >(str[i + j]);
            if ((next_c & 0xC0) != 0x80) return false;
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
        unsigned char c1 = static_cast< unsigned char >(str[i]);
        if (c1 <= 0x7F) {
            i++;
            continue;    // ASCII 字符
        }
        // GBK 双字节规则：
        // 第一个字节：0x81-0xFE（除 0xA1-0xA9 是符号区，但仍属合法）
        // 第二个字节：0x40-0x7E 或 0x80-0xFE
        if (i + 1 >= str.size( )) return false;    // 不完整的双字节
        unsigned char c2 = static_cast< unsigned char >(str[i + 1]);
        if ((c1 >= 0x81 && c1 <= 0xFE) && ((c2 >= 0x40 && c2 <= 0x7E) || (c2 >= 0x80 && c2 <= 0xFE))) {
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
 * @brief utf8转wstring
 * @param utf8编码的string
 * @return 宽字符wstring（win里面实际是utf-16）
 * @note 只能在windows编译
 **/
std::wstring utf8_to_wstring_win(const std::string u8) {
    if (u8.empty( )) return { };
    int          len = MultiByteToWideChar(CP_UTF8, 0, u8.c_str( ), -1, nullptr, 0);
    std::wstring out(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, u8.c_str( ), -1, &out[0], len);
    out.pop_back( );    // 去掉结尾 L'\0'
    return out;
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
