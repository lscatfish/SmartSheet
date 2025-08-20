/**
 * @file icu_encoding_handler.cpp
 * @brief ICU 77.1编码处理工具实现
 */

#include <cstring>
#include <icu_encoding_handler.h>
#include <iostream>
#include <string>
#include <unicode/ucnv.h>
#include <unicode/ucnv_err.h>
#include <unicode/ucsdet.h>
#include <unicode/uloc.h>
#include <unicode/unistr.h>
#include <vector>
#include <windows.h>


namespace ICUEncodingHandler {

/**
 * @brief 检测字符串的编码格式
 * @param data 待检测的字节数据
 * @param dataLen 数据长度
 * @param results 输出参数，存储检测结果
 * @param maxResults 最大返回结果数量
 * @return 成功返回true，失败返回false
 */
bool detect_encoding(const char *data, int32_t dataLen,
                     std::vector< EncodingDetectionResult > &results,
                     int32_t                                 maxResults) {
    if (!data || dataLen <= 0 || maxResults <= 0) {
        return false;
    }

    UErrorCode        status   = U_ZERO_ERROR;
    UCharsetDetector *detector = ucsdet_open(&status);
    if (U_FAILURE(status)) {
        return false;
    }

    // 设置检测数据
    ucsdet_setText(detector, data, dataLen, &status);
    if (U_FAILURE(status)) {
        ucsdet_close(detector);
        return false;
    }

    // 获取检测结果
    int32_t               matchCount = 0;
    const UCharsetMatch **matches    = ucsdet_detectAll(detector, &matchCount, &status);
    if (U_FAILURE(status) || !matches) {
        ucsdet_close(detector);
        return false;
    }

    // 处理结果（适配ICU 77.1的ucsdet_getConfidence参数要求）
    int32_t resultCount = (std::min)(matchCount, maxResults);
    for (int32_t i = 0; i < resultCount; ++i) {
        EncodingDetectionResult result;
        UErrorCode              confStatus = U_ZERO_ERROR;

        // ICU 77.1要求ucsdet_getConfidence必须传入status参数
        result.confidence   = static_cast< float >(ucsdet_getConfidence(matches[i], &confStatus)) / 100.0f;
        result.encodingName = ucsdet_getName(matches[i], &status);

        // ICU 77.1中已移除ucsdet_getHasBOM，通过编码名称判断是否可能有BOM
        result.hasBom = (result.encodingName.find("UTF-8") != std::string::npos) || (result.encodingName.find("UTF-16") != std::string::npos) || (result.encodingName.find("UTF-32") != std::string::npos);

        results.push_back(result);
    }

    ucsdet_close(detector);
    return true;
}

/**
 * @brief 将指定编码的字符串转换为UTF-8编码
 * @param srcData 源数据
 * @param srcDataLen 源数据长度
 * @param srcEncoding 源数据编码
 * @param destUtf8 输出参数，存储转换后的UTF-8字符串
 * @param errorMsg 输出参数，存储错误信息
 * @return 成功返回true，失败返回false
 */
bool convert_to_utf8(const char *srcData, int32_t srcDataLen,
                     const std::string &srcEncoding,
                     std::string       &destUtf8,
                     std::string       &errorMsg) {
    // 入参校验
    if (!srcData || srcDataLen <= 0 || srcEncoding.empty( )) {
        errorMsg = u8"无效的输入参数";
        return false;
    }

    UErrorCode  status    = U_ZERO_ERROR;
    UConverter *converter = ucnv_open(srcEncoding.c_str( ), &status);
    if (U_FAILURE(status)) {
        errorMsg = u8"创建转换器失败: " + std::string(u_errorName(status));
        return false;
    }

    icu::UnicodeString unicodeStr;
    const char        *srcEnd             = srcData + srcDataLen;
    const char        *srcCurrent         = srcData;
    UChar             *destBuffer         = nullptr;
    UChar             *originalDestBuffer = nullptr;    // 保存缓冲区原始地址（关键修复）
    int32_t            destCapacity       = 0;

    try {
        // 第一次调用：获取所需缓冲区大小（此时不分配实际内存）
        ucnv_toUnicode(converter, &destBuffer, nullptr,    // 目标缓冲区为nullptr时仅计算大小
                       &srcCurrent, srcEnd, nullptr, true, &status);

        // 处理缓冲区溢出（正常流程，说明需要分配内存）
        if (status == U_BUFFER_OVERFLOW_ERROR) {
            status = U_ZERO_ERROR;
            // 计算安全的缓冲区大小：ICU推荐使用源长度×4（应对所有编码的最坏情况）
            destCapacity       = srcDataLen * 4;
            destBuffer         = new UChar[destCapacity];
            originalDestBuffer = destBuffer;    // 保存原始地址
            srcCurrent         = srcData;       // 重置源数据指针

            // 第二次调用：实际转换（使用分配的缓冲区）
            ucnv_toUnicode(converter, &destBuffer, originalDestBuffer + destCapacity,    // 正确的缓冲区上限
                           &srcCurrent, srcEnd, nullptr, true, &status);

            // 检查转换结果
            if (U_FAILURE(status)) {
                errorMsg = u8"转换到Unicode失败: " + std::string(u_errorName(status));
                throw std::runtime_error(errorMsg);
            }

            // 计算实际转换的长度（关键修复：用原始地址计算差值）
            int32_t actualLen = static_cast< int32_t >(destBuffer - originalDestBuffer);
            if (actualLen < 0 || actualLen > destCapacity) {
                errorMsg = u8"转换结果长度异常（可能内存访问越界）";
                throw std::runtime_error(errorMsg);
            }

            // 安全地初始化Unicode字符串
            unicodeStr.setTo(originalDestBuffer, actualLen);

            // 释放缓冲区（仅释放一次）
            delete[] originalDestBuffer;
            originalDestBuffer = nullptr;
            destBuffer         = nullptr;
        }
        // 处理其他错误
        else if (U_FAILURE(status)) {
            errorMsg = u8"获取缓冲区大小失败: " + std::string(u_errorName(status));
            throw std::runtime_error(errorMsg);
        }

        // 确认所有源数据都被处理
        if (srcCurrent != srcEnd) {
            errorMsg = u8"未处理完所有源数据（可能缓冲区不足）";
            throw std::runtime_error(errorMsg);
        }

        // 转换为UTF-8并返回
        unicodeStr.toUTF8String(destUtf8);
        ucnv_close(converter);
        return true;
    }
    // 异常路径：确保所有资源被释放
    catch (...) {
        if (originalDestBuffer != nullptr) {
            delete[] originalDestBuffer;    // 释放原始缓冲区（避免重复释放）
            originalDestBuffer = nullptr;
            destBuffer         = nullptr;
        }
        ucnv_close(converter);
        return false;
    }
}

bool convert_from_utf8(const char *utf8Data, int32_t utf8DataLen,
                       const std::string &destEncoding,
                       std::string       &destData,
                       std::string       &errorMsg) {
    if (!utf8Data || utf8DataLen <= 0 || destEncoding.empty( )) {
        errorMsg = u8"无效的输入参数";
        return false;
    }

    // 1. 正确构建 UnicodeString：用静态 fromUTF8，不依赖 UErrorCode 参数
    icu::StringPiece   sp(utf8Data, utf8DataLen);
    icu::UnicodeString unicodeStr = icu::UnicodeString::fromUTF8(sp);

    // 检查是否为 "bogus" 字符串（严重解析错误）
    if (unicodeStr.isBogus( )) {
        errorMsg = u8"UTF-8 数据解析失败，结果为无效字符串";
        return false;
    }

    // 2. 创建目标编码转换器
    UErrorCode  status    = U_ZERO_ERROR;
    UConverter *converter = ucnv_open(destEncoding.c_str( ), &status);
    if (U_FAILURE(status)) {
        errorMsg = u8"创建转换器失败: " + std::string(u_errorName(status));
        return false;
    }

    char *destBuffer = nullptr;
    try {
        // 计算缓冲区大小：Unicode 字符数 × 目标编码最大单字符字节数 + 1（结束符）
        int32_t maxCharSize  = ucnv_getMaxCharSize(converter);
        int32_t destCapacity = unicodeStr.length( ) * maxCharSize + 1;
        if (destCapacity <= 0) {
            errorMsg = u8"计算缓冲区大小失败，结果无效";
            ucnv_close(converter);
            return false;
        }

        destBuffer               = new char[destCapacity];
        char        *destCurrent = destBuffer;
        const UChar *srcBuffer   = unicodeStr.getBuffer( );
        const UChar *srcEnd      = srcBuffer + unicodeStr.length( );

        // 3. 执行转换
        ucnv_fromUnicode(converter, &destCurrent, destBuffer + destCapacity,
                         &srcBuffer, srcEnd, nullptr, true, &status);

        // 4. 校验转换结果
        if (U_FAILURE(status)) {
            errorMsg = u8"转换到目标编码失败: " + std::string(u_errorName(status));
            delete[] destBuffer;
            ucnv_close(converter);
            return false;
        }
        if (srcBuffer != srcEnd) {
            errorMsg = u8"部分 UTF-8 数据未完成转换";
            delete[] destBuffer;
            ucnv_close(converter);
            return false;
        }
        if (destCurrent < destBuffer || destCurrent >= destBuffer + destCapacity) {
            errorMsg = u8"转换结果超出缓冲区范围，可能导致内存问题";
            delete[] destBuffer;
            ucnv_close(converter);
            return false;
        }

        // 5. 收尾：添加结束符 + 赋值结果
        *destCurrent = '\0';
        destData     = std::string(destBuffer, destCurrent - destBuffer);

        // 释放资源
        delete[] destBuffer;
        ucnv_close(converter);
        return true;

    } catch (...) {
        if (destBuffer != nullptr) {
            delete[] destBuffer;
        }
        ucnv_close(converter);
        errorMsg = u8"转换过程中发生未知异常";
        return false;
    }
}

/**
 * @brief 获取系统默认编码
 * @return 系统默认编码名称
 */
std::string get_system_default_encoding( ) {
    // 调用ICU 77.1的ucnv_getDefaultName函数获取默认编码
    // 该函数无参数，直接返回编码名称字符串
    const char *defaultEncoding = ucnv_getDefaultName( );

    // 检查返回值有效性
    if (!defaultEncoding || *defaultEncoding == '\0') {
        // 失败时返回Windows默认ANSI编码作为 fallback
        return "windows-936";    // 通常对应GBK编码
    }

    return std::string(defaultEncoding);
}

}    // namespace ICUEncodingHandler
