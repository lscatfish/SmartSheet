/* =================================================================================
 * @file    chstring.cpp
 *
 * @attention [2025.08.29][@lscatfish]此hpp尚未集成到项目中
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
#include <basic.hpp>
#include <chstring.hpp>
#include <Encoding.h>
#include <iostream>

// 比较运算符实现
bool chstring::operator==(const chstring &b) const {
    return u8str == b.u8str;
}
bool chstring::operator>(const chstring &b) const {
    return u8str > b.u8str;
}
bool chstring::operator<(const chstring &b) const {
    return u8str < b.u8str;
}
bool chstring::operator>=(const chstring &b) const {
    return !(*this < b);
}
bool chstring::operator<=(const chstring &b) const {
    return !(*this > b);
}

// 访问指定位置的字符
char &chstring::operator[](size_t pos) {
    return this->u8str[pos];
}
const char &chstring::operator[](size_t pos) const {
    return this->u8str[pos];
}

// 字符串拼接实现（修正返回类型为chstring）
chstring chstring::operator+(const chstring &b) const {
    return chstring(u8str + b.u8str);
}

// 迭代器相关方法
chstring::iterator chstring::begin( ) {
    return this->u8str.begin( );
}    // 返回起始迭代器
chstring::iterator chstring::end( ) {
    return this->u8str.end( );
}    // 返回结束迭代器
chstring::const_iterator chstring::begin( ) const {
    return this->u8str.begin( );
}    // 常量版本起始迭代器
chstring::const_iterator chstring::end( ) const {
    return this->u8str.end( );
}    // 常量版本结束迭代器
chstring::const_iterator chstring::cbegin( ) const {
    return this->u8str.cbegin( );
}    // 常量迭代器（C++11）
chstring::const_iterator chstring::cend( ) const {
    return this->u8str.cend( );
}    // 常量迭代器（C++11）

// 获取长度
size_t chstring::length( ) const {
    return this->u8str.length( );
}

// 检查是否为空
bool chstring::empty( ) const {
    return this->u8str.empty( );
}

// 获取大小
size_t chstring::size( ) const {
    return this->u8str.size( );
}

// 获取底层字符串
std::string chstring::get_u8string( ) const {
    return this->u8str;
}

std::string chstring::get_sysstring( ) const {
    return this->sysstr;
}
