
/* ===============================================================================================================
 * @file    chstring.hpp
 *
 * 设计初衷：希望可以自动管理中文的编码方式，目前没有集成到
 *
 * @brief 用于操作中文编码的string
 * @note 没有从string派生是因为string没有虚析构函数，因此通过基类指针删除派生类对象可能导致未定义行为
 * -------250826----------$$$@note 希望此类可以集成到icu，以达到对中文编码的精确识别$$$--------------------
 * @note [2025.08.26][@lscatfish]已集成icu
 *
 * @todo [2025.08.26][@lscatfish]希望将此hpp集成到项目中，所有的字符串都由此类实现
 *
 * 作者：lscatfish
 * 邮箱：2561925435@qq.com
 * =============================================================================================================== */
#pragma once

#ifndef CHSTRING_HPP
#define CHSTRING_HPP

#include <Encoding.h>
#include <iostream>
#include <corecrt_wstring.h>
#include <string>
#include <utility>

// chstring不允许对单个char的大小进行操作
class chstring {
private:
    // 表格
    template < typename _T >
    using myTable = std::vector< std::vector< _T > >;    // 如果冲突，请封装到namespace中，或者改名为 MyTable

    // 一行（列）
    template < typename _T >
    using myList = std::vector< _T >;    // 如果冲突，请封装到namespace中，或者改名为 MyList

public:
    enum class csType {
        UTF8 = 0,    // utf8编码
        SYS  = 1,    // 系统默认的编码形式
    };

    // 按照字符u8的降序比较
    struct CompareByUTF8Desc {
        bool operator( )(const chstring &a, const chstring &b) const {
            chstring u8a(a, csType::UTF8);
            chstring u8b(b, csType::UTF8);
            return u8a > u8b;
        }
    };

    // 按照字符u8的降序比较
    struct CompareByUTF8Asc {
        bool operator( )(const chstring &a, const chstring &b) const {
            chstring u8a(a, csType::UTF8);
            chstring u8b(b, csType::UTF8);
            return u8a < u8b;
        }
    };

    // 按照字符sys的降序
    struct CompareBySysDesc {
        bool operator( )(const chstring &a, const chstring &b) const {
            chstring sysa(a, csType::SYS);
            chstring sysb(b, csType::SYS);
            return sysa > sysb;
        }
    };

    // 按照字符sys的升序
    struct CompareBySysAsc {
        bool operator( )(const chstring &a, const chstring &b) const {
            chstring sysa(a, csType::SYS);
            chstring sysb(b, csType::SYS);
            return sysa < sysb;
        }
    };

    chstring(const std::string &_in_, const csType _inType = csType::UTF8);
    chstring(const char *cstr, const csType _inType = csType::UTF8);
    chstring(const chstring &_in_);
    chstring(const chstring &_in_, const csType _inType);
    chstring( );
    chstring(const csType _inType);
    ~chstring( ) = default;


    // 交换a，b的内容,交换指针
    static void swap(chstring &a, chstring &b);

    // 产生u8list
    static myList< std::string > get_u8list(const myList< chstring > &_inList);

    // 产生u8table
    static myTable< std::string > get_u8table(const myTable< chstring > &_inTable);

    // 产生syslist
    static myList< std::string > get_syslist(const myList< chstring > &_inList);

    // 产生systable
    static myTable< std::string > get_systable(const myTable< chstring > &_inTable);

    // 产生chstringlist
    static myList< chstring > get_cslist(const myList< std::string > &_inList);

    // 产生chstringtable
    static myTable< chstring > get_cstable(const myTable< std::string > &_inList);

    // 定义迭代器类型，使用std::string的迭代器作为底层实现
    using iterator               = std::string::iterator;
    using reverse_iterator       = std::string::reverse_iterator;
    using const_iterator         = std::string::const_iterator;
    using const_reverse_iterator = std::string::const_reverse_iterator;

    // 友元函数：输出运算符重载
    friend std::ostream &operator<<(std::ostream &os, const chstring &cs) {
        std::string u8 = cs.u8string( );
        os << u8;
        return os;
    }

    // 比较运算符重载
    // 比较，总是将utf8放在sys之前(utf8<sys)
    bool operator==(const chstring &b) const;
    bool operator|=(const chstring &b) const;    // 字符实质是否相等
    bool operator>(const chstring &b) const;
    bool operator<(const chstring &b) const;
    bool operator>=(const chstring &b) const;
    bool operator<=(const chstring &b) const;

    // 访问指定位置的字符
    char       &operator[](size_t pos);
    const char &operator[](size_t pos) const;

    // 字符串拼接运算符重载(总是匹配到前一个字符的编码形式)
    chstring  operator+(const chstring &b) const;
    chstring  operator+(const std::string &b) const;
    chstring  operator+(const char *b) const;
    chstring &operator+=(const chstring &b);
    chstring &operator+=(const std::string &b);
    chstring &operator+=(const char *b);
    chstring &operator=(const chstring &other);

    // 迭代器相关方法
    iterator               begin( );    // 返回起始迭代器
    iterator               end( );      // 返回结束迭代器
    reverse_iterator       rbegin( );
    reverse_iterator       rend( );
    const_iterator         begin( ) const;     // 常量版本起始迭代器
    const_iterator         end( ) const;       // 常量版本结束迭代器
    const_iterator         cbegin( ) const;    // 常量迭代器（C++11）
    const_iterator         cend( ) const;      // 常量迭代器（C++11）
    const_reverse_iterator crbegin( ) const;
    const_reverse_iterator crend( ) const;

    // 长度，不同的编码不同
    size_t length( ) const;

    // 检查是否为空
    bool empty( ) const;

    // 获取大小，不同的编码不同
    size_t size( ) const;

    // 删除最后一位
    void pop_back( );

    // 擦除其中内容
    void erase( );

    // 获取字串
    chstring substr(size_t pos = 0, size_t count = std::string::npos) const;

    // 末尾的字符
    const char &back( ) const;

    // 打印文字到控制台
    void print( ) const;

    // 打赢文字与回车到控制台
    void println( ) const;

    // 获取底层字符串，按照u8编码返回
    std::string u8string( ) const;

    // 获取底层字符串，按照系统编码返回
    std::string sysstring( ) const;

    // 获取底层字符串，按照当前编码返回
    std::string usstring( ) const;

    // 获取宽字符方式
    std::wstring wstring( ) const;

    // 获取当前使用的文字的编码类型
    csType cstype( ) const;

    // 转化编码
    void cvtEncode_to(csType _to);

    // 清除字符串前后的所有空白字符（包括空格、\t、\n等）
    void trim_whitespace( );

    // 单获取清除字符串前后的所有空白字符（包括空格、\t、\n等）
    chstring get_trim_whitespace( ) const;

    // 分离中文与数字
    std::pair< chstring, chstring > split_chinese_and_number( ) const;

    // 分离键与值
    std::pair< chstring, chstring > split_by_equal( ) const;

    // 按照第一个char分离
    std::pair< chstring, chstring > split_by_first_of(const char cutter) const;

    // 按照最后一个char分离
    std::pair< chstring, chstring > split_by_last_of(const char cutter) const;

    // 按照第一个char分离
    std::pair< chstring, chstring > split_by_first_of(const std::string &cutter) const;

    // 按照最后一个char分离
    std::pair< chstring, chstring > split_by_last_of(const std::string &cutter) const;

    // 分离前后缀
    std::pair< chstring, chstring > split_filename_and_extension( ) const;

    // 是否都是数字
    bool is_all_digits( ) const;

    // 是否含有子串
    bool has_subchstring(const chstring &_substr) const;


private:
    std::string usingStr_;    // 当前使用的字符串（编码同usingType）
    csType      usingType_;
};



#endif    // !CHSTRING_HPP
