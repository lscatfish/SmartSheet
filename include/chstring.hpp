
/* =================================================================================
 * @file    chstring.hpp
 *
 * @brief 用于操作中文编码的文件
 * @note 没有从string派生是因为string没有虚析构函数，因此通过基类指针删除派生类对象可能导致未定义行为
 * -------250826----------$$$@note 希望此类可以集成到icu，以达到对中文编码的精确识别$$$--------------------
 * @note [2025.08.26][@lscatfish]已集成icu
 *
 * @todo [2025.08.26][@lscatfish]希望将此hpp集成到项目中，所有的字符串都由此类实现
 *
 * 作者：lscatfish
 * 邮箱：2561925435@qq.com
 * ================================================================================= */

#pragma once

#ifndef CHSTRING_HPP
#define CHSTRING_HPP

#include <basic.hpp>
#include <Encoding.h>
#include <iostream>
#include <corecrt_wstring.h>
#include <string>
#include <utility>

class chstring {
public:
    enum class CStype {
        UTF8 = 0,    // utf8编码
        SYS  = 1,    // 系统默认的编码形式
    };

    chstring(const std::string &_in_, CStype _inType = CStype::UTF8) {
        if (_inType == CStype::UTF8)
            this->usingStr_ = encoding::sysdcode_to_utf8(_in_);    // 使用Encoding库转换编码
        else if (_inType == CStype::SYS)
            this->usingStr_ = encoding::utf8_to_sysdcode(_in_);
        usingType_ = _inType;
    }
    chstring(const char *cstr, CStype _inType = CStype::UTF8) {
        if (_inType == CStype::UTF8)
            this->usingStr_ = encoding::sysdcode_to_utf8(std::string(cstr));    // 使用Encoding库转换编码
        else if (_inType == CStype::SYS)
            this->usingStr_ = encoding::utf8_to_sysdcode(std::string(cstr));    // 使用Encoding库转换编码
        usingType_ = _inType;
    }
    chstring(const chstring &_in_) {
        *this = _in_;    // 直接复制底层
    }
    chstring(const chstring &_in_, CStype _inType) {
        *this = _in_;    // 直接复制底层
        cvtEncode(_inType);
    }
    chstring( ) {
        this->usingStr_  = "";
        this->usingType_ = CStype::UTF8;
    }
    ~chstring( ) = default;

    // 定义迭代器类型，使用std::string的迭代器作为底层实现
#if false
    using iterator       = std::string::iterator;
#endif
    using const_iterator = std::string::const_iterator;

    // 友元函数：输出运算符重载
    friend std::ostream &operator<<(std::ostream &os, const chstring &cs) {
        std::string u8 = encoding::sysdcode_to_utf8(cs.usingStr_);
        os << u8;
        return os;
    }

    // 比较运算符重载
    // 比较，总是将utf8放在sys之前(utf8<sys)
    bool operator==(const chstring &b) const;
    bool operator>(const chstring &b) const;
    bool operator<(const chstring &b) const;
    bool operator>=(const chstring &b) const;
    bool operator<=(const chstring &b) const;

#if false
    char       &operator[](size_t pos);
#endif
    // 访问指定位置的字符
    const char &operator[](size_t pos) const;

    // 字符串拼接运算符重载(总是匹配到前一个字符的编码形式)
    chstring operator+(const chstring &b) const;
    chstring operator+(const std::string &b) const;

#if false
    iterator       begin( );           // 返回起始迭代器
    iterator       end( );             // 返回结束迭代器
#endif
    // 迭代器相关方法
    const_iterator begin( ) const;     // 常量版本起始迭代器
    const_iterator end( ) const;       // 常量版本结束迭代器
    const_iterator cbegin( ) const;    // 常量迭代器（C++11）
    const_iterator cend( ) const;      // 常量迭代器（C++11）

    // 长度，不同的编码不同
    size_t length( ) const;

    // 检查是否为空
    bool empty( ) const;

    // 获取大小，不同的编码不同
    size_t size( ) const;

    // 获取底层字符串，按照u8编码返回
    std::string get_u8string( ) const;

    // 获取底层字符串，按照系统编码返回
    std::string get_sysstring( ) const;

    // 获取宽字符方式
    std::wstring get_wstring( ) const;

    // 获取当前使用的文字的编码类型
    CStype get_encoding_type( ) const;

    // 转化编码
    void cvtEncode_to(CStype _to);

    // 清除字符串前后的所有空白字符（包括空格、\t、\n等）
    void trim_whitespace( );

    // 分离中文与数字
    std::pair< chstring, chstring > split_chinese_and_number( ) const;

    // 分离键与值
    std::pair< chstring, chstring > split_by_equal( ) const;

    // 是否都是数字
    bool is_all_digits( ) const;



    // 交换a，b的内容,交换指针
    static void swap(chstring &a, chstring &b);



private:
    std::string usingStr_;    // 当前使用的字符串（编码同usingType）
    CStype      usingType_;
};



#endif    // !CHSTRING_HPP
