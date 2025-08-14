#include <chstring.hpp>
#include <Encoding.h>
#include <iostream>


// 比较运算符实现
bool chstring::operator==(const chstring &b) const {
    return str == b.str;
}
bool chstring::operator>(const chstring &b) const {
    return str > b.str;
}
bool chstring::operator<(const chstring &b) const {
    return str < b.str;
}
bool chstring::operator>=(const chstring &b) const {
    return !(*this < b);
}
bool chstring::operator<=(const chstring &b) const {
    return !(*this > b);
}

// 访问指定位置的字符
char &chstring::operator[](size_t pos) {
    return this->str[pos];
}
const char &chstring::operator[](size_t pos) const {
    return this->str[pos];
}

// 字符串拼接实现（修正返回类型为chstring）
chstring chstring::operator+(const chstring &b) const {
    return chstring(str + b.str);
}

// 迭代器相关方法
chstring::iterator chstring::begin( ) {
    return this->str.begin( );
}    // 返回起始迭代器
chstring::iterator chstring::end( ) {
    return this->str.end( );
}    // 返回结束迭代器
chstring::const_iterator chstring::begin( ) const {
    return this->str.begin( );
}    // 常量版本起始迭代器
chstring::const_iterator chstring::end( ) const {
    return this->str.end( );
}    // 常量版本结束迭代器
chstring::const_iterator chstring::cbegin( ) const {
    return this->str.cbegin( );
}    // 常量迭代器（C++11）
chstring::const_iterator chstring::cend( ) const {
    return this->str.cend( );
}    // 常量迭代器（C++11）

// 获取长度
size_t chstring::length( ) const {
    return this->str.length( );
}

// 检查是否为空
bool chstring::empty( ) const {
    return this->str.empty( );
}

// 获取大小
size_t chstring::size( ) const {
    return this->str.size( );
}

// 获取底层字符串
std::string chstring::get_string( ) const {
    return this->str;
}
