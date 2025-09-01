// Created on: 2024-06-09 15:26:49
/*******************************************************************************
 * @file    Encoding.cpp
 *
 * @note [2025.08.26][@lscatfish]已集成icu
 *
 * @todo [2025.08.26][@lscatfish]希望将此hpp集成到项目中，所有的字符串都由此类实现
 *
 * 作者：lscatfish
 *****************************************************************************/

#include <basic.hpp>
#include <consoleapi2.h>
#include <cstdint>
#include <cstring>
#include <Encoding.h>
#include <helper.h>
#include <icu_encoding_handler.h>
#include <interlockedapi.h>    // PSLIST_HEADER
#include <iostream>
#include <stdexcept>
#include <string>
#include <stringapiset.h>
#include <unicode/ucnv.h>
#include <unicode/ucnv_err.h>
#include <unicode/umachine.h>
#include <unicode/utf16.h>
#include <unicode/utf8.h>
#include <unicode/utypes.h>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include <WinNls.h>
#include <winnt.h>    // CONTEXT, PCONTEXT
#endif                //  _WIN32

namespace encoding {

// 系统默认的编码方式
std::string systemDefaultEncoding = "windows-936";

// 启动函数，配置编码
void Init( ) {
    systemDefaultEncoding = ICUEncodingHandler::get_system_default_encoding( );
}

/*
 * @brief utf8转wstring
 * @param u8 utf8编码的string
 * @return 宽字符wstring（win里面实际是utf-16）
 * @note 只能在windows编译,可以考虑兼容到icu
 **/
// std::wstring utf8_to_wstring(const std::string u8) {
//    if (u8.empty( )) return { };
//    int          len = MultiByteToWideChar(CP_UTF8, 0, u8.c_str( ), -1, nullptr, 0);
//    std::wstring out(len, 0);
//    MultiByteToWideChar(CP_UTF8, 0, u8.c_str( ), -1, &out[0], len);
//    out.pop_back( );    // 去掉结尾 L'\0'
//    return out;
// }
/*
 * @brief utf8转wstring
 * @param u8 utf8编码的string
 * @return 宽字符wstring（win里面实际是utf-16）
 * @note 基于 ICU77.1
 **/
std::wstring utf8_to_wstring(const std::string u8) {
    if (u8.empty( )) return { };

    UErrorCode  status = U_ZERO_ERROR;
    UConverter *cnv    = ucnv_open("UTF-8", &status);
    if (U_FAILURE(status)) {
        std::cout << "\n"
                  << u8 << "   "
                  << U8C(u8"ICU: 无法创建 UTF-8 转换器") << "\n";
        throw std::runtime_error(U8C(u8"ICU: 无法创建 UTF-8 转换器"));
    }

    // 1. 预查询所需 UTF-16 单元数（不含终止 0）
    int32_t dst_len = ucnv_toUChars(cnv, nullptr, 0,
                                    u8.data( ), static_cast< int32_t >(u8.size( )),
                                    &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) status = U_ZERO_ERROR;

    // 2. 一次性转换
    std::wstring out(dst_len, L'\0');
    ucnv_toUChars(cnv,
                  reinterpret_cast< UChar * >(out.data( )),
                  dst_len + 1,    // 关键：+1 避免 ICU 认为空间不足
                  u8.data( ), static_cast< int32_t >(u8.size( )),
                  &status);

    ucnv_close(cnv);

    if (U_FAILURE(status)) {
        std::cout << "\n"
                  << u8 << "    "
                  << U8C(u8"ICU: UTF-8 → UTF-16 转换失败") << "\n";
        throw std::runtime_error(U8C(u8"ICU: UTF-8 → UTF-16 转换失败"));
    }

    return out;
}

/**
 * @brief 删除字符串中的所有ASCII字符（ASCII码0-127）
 * @param input 输入字符串，可能包含ASCII和非ASCII字符
 * @return 处理后的字符串，仅保留非ASCII字符
 *
 * @todo 这个函数对多字符的字体删除有极大的问题
 *
 */
static std::string remove_ascii_characters(const std::string &input) {
    std::string result;
    // 预留足够空间，避免频繁内存分配
    result.reserve(input.size( ));

    // 遍历输入字符串的每个字符
    for (char c : input) {
        // ASCII字符的范围是0-127
        if (static_cast< unsigned char >(c) > 127) {
            // 非ASCII字符，保留
            result += c;
        }
        // ASCII字符会被自动跳过
    }

    return result;
}

/*
 * @brief 将系统默认的中文（简体）编码转化为utf8
 * @param _anycode 任意中文（简体）格式的string
 * @return 转化为utf8格式的string
 */
std::string sysdcode_to_utf8(const std::string &_anycode) {
    std::string out;    // 输出
    std::string e;      // 错误
    if (is_utf8(_anycode))
        return _anycode;
    if (ICUEncodingHandler::convert_to_utf8(_anycode.c_str( ), _anycode.size( ), systemDefaultEncoding, out, e)) {
        return out;
    } else {
        std::cout << std::endl
                  << e << std::endl;
        pause( );
        return "encoding error";
    }
}

/*
 * @brief 将utf8转化为系统默认的中文（简体）编码
 * @param _anycode 任意u8格式的string
 * @return 转化为系统格式格式的string
 */
std::string utf8_to_sysdcode(const std::string &_u8) {
    if (!is_utf8(_u8)) {
        return _u8;
    }
    std::string out;
    std::string e;
    if (ICUEncodingHandler::convert_from_utf8(_u8.c_str( ), _u8.size( ), systemDefaultEncoding, out, e)) {
        return out;
    } else {
        std::cout << std::endl
                  << e << std::endl;
        pause( );
        return "encoding error";
    }
}

/*
 * @brief 检测字符串的编码格式是否为utf8
 * @param _u8 待检测的字符串
 * @return 是utf8返回true，否则返回false
 */
bool is_utf8(const std::string &_u8) {
    std::string ys = _u8;
    while (ys.size( ) < 100) {
        ys = ys + ys;
    }
    std::vector< EncodingDetectionResult > icuResult;
    if (ICUEncodingHandler::detect_encoding(ys.c_str( ), ys.size( ), icuResult, 1)) {
        if (icuResult[0].encodingName == "UTF-8")
            return true;
        else
            return false;
    } else {
        return false;
    }
}

/*
 * @brief 检测utf8字符串是否有错误
 * @param utf8 要检测的utf8字符串
 * @return 返回是否有效
 */
bool check_utf8_validity(const std::string &utf8) {
    ICUEncodingHandler::ValidationResult result = ICUEncodingHandler::check_utf8_validity(utf8);
    if (result.hasError) {    // 有错误
        return false;         // 不有效
    }
    return true;
}

/*
 * @brief 检测一个table<string>内部是否有非法字符集
 * @param _sheet 输入的sheet
 * @return 返回是否有效
 */
bool check_sheet_utf8_validity(const table< std::string > &_sheet) {
    for (const auto &r : _sheet) {
        for (const auto &c : r) {
            if (!check_utf8_validity(c)) {
                return false;
            }
        }
    }
}

/*
 * @brief 修复table<string>内部的非法字符集错误
 * @param _sheet 输入的sheet
 * @return 返回修复的字符集的错误数量
 */
size_t repair_sheet_utf8_invalidity(table< std::string > &_sheet) {
    size_t sum = 0;
    for (auto &row : _sheet) {
        for (auto &cell : row) {
            if (!check_utf8_validity(cell)) {
                sum++;
                cell = U8C(u8"utf8-ERROR");
            }
        }
    }
    return sum;
}


}    // namespace encoding