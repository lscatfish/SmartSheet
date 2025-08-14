
/* =================================================================================
 * @file    chstring.hpp
 *
 * @brief 用于操作中文编码的文件
 * @note 没有从string派生是因为string没有虚析构函数，因此通过基类指针删除派生类对象可能导致未定义行为
 * 
 * 作者：刘思成
 * 邮箱：2561925435@qq.com
 * ================================================================================= */

#pragma once

#include <iostream>
#include <string>
#include <Encoding.h>

#ifndef CHSTRING_HPP
#define CHSTRING_HPP

class chstring {
public:

    chstring(std::string _in_) {
        this->str = encoding::chcode_to_utf8(_in_);    // 使用Encoding库转换编码
    }
    chstring(const char *cstr) {
        this->str = encoding::chcode_to_utf8(std::string(cstr));    // 使用Encoding库转换编码
    }
    chstring(const chstring &_in_) {
        this->str = _in_.str;    // 直接复制底层字符串
    }
    ~chstring( ) {}

    // 定义迭代器类型，使用std::string的迭代器作为底层实现
    using iterator       = std::string::iterator;
    using const_iterator = std::string::const_iterator;

    // 友元函数：输出运算符重载
    friend std::ostream &operator<<(std::ostream &os, const chstring &cs) {
        os << cs.str;
        return os;
    }

    // 比较运算符重载
    bool operator==(const chstring &b) const;
    bool operator>(const chstring &b) const;
    bool operator<(const chstring &b) const;
    bool operator>=(const chstring &b) const;
    bool operator<=(const chstring &b) const;

    // 访问指定位置的字符
    char       &operator[](size_t pos);
    const char &operator[](size_t pos) const;

    // 字符串拼接运算符重载
    chstring operator+(const chstring &b) const;

    // 迭代器相关方法
    iterator       begin( );           // 返回起始迭代器
    iterator       end( );             // 返回结束迭代器
    const_iterator begin( ) const;     // 常量版本起始迭代器
    const_iterator end( ) const;       // 常量版本结束迭代器
    const_iterator cbegin( ) const;    // 常量迭代器（C++11）
    const_iterator cend( ) const;      // 常量迭代器（C++11）

    // 长度
    size_t length( ) const;

    // 检查是否为空
    bool empty( ) const;

    // 获取大小
    size_t size( ) const;

    // 获取底层字符串
    std::string get_string( ) const;

private:
    std::string str;
};



#endif    // !CHSTRING_HPP
