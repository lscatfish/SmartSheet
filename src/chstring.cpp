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
#include <string>


// 转化编码
void chstring::cvtEncode(CStype _in) {
    if (_in == usingType_)
        return;
    else if (_in == CStype::SYS)
        this->usingStr_ = encoding::utf8_to_sysdcode(this->usingStr_);
    else if (_in == CStype::UTF8)
        this->usingStr_ = encoding::sysdcode_to_utf8(this->usingStr_);
}

// 比较运算符实现
bool chstring::operator==(const chstring &b) const {
    if (this->usingType_ == b.usingType_)
        return this->usingStr_ == b.usingStr_;
    else
        return false;
}
bool chstring::operator>(const chstring &b) const {
    if (this->usingType_ == b.usingType_)
        return this->usingStr_ > b.usingStr_;
    else
        return this->usingType_ > b.usingType_;
}
bool chstring::operator<(const chstring &b) const {
    if (this->usingType_ == b.usingType_)
        return this->usingStr_ < b.usingStr_;
    else
        return this->usingType_ < b.usingType_;
}
bool chstring::operator>=(const chstring &b) const {
    return !(*this < b);
}
bool chstring::operator<=(const chstring &b) const {
    return !(*this > b);
}

// 访问指定位置的字符
char &chstring::operator[](size_t pos) {
    return this->usingStr_[pos];
}
const char &chstring::operator[](size_t pos) const {
    return this->usingStr_[pos];
}

// 字符串拼接实现（修正返回类型为chstring）(总是匹配到前一个字符的编码形式)
chstring chstring::operator+(const chstring &b) const {
    if (this->usingType_ == b.usingType_)
        return chstring(this->usingStr_ + b.usingStr_, this->usingType_);
    else {
        chstring toFirst(b, this->usingType_);
        return chstring(this->usingStr_ + toFirst.usingStr_, this->usingType_);
    }
}

// 字符串拼接实现（修正返回类型为chstring）(总是匹配到前一个字符的编码形式)
chstring chstring::operator+(const std::string &b) const {
    if (this->usingType_ == CStype::SYS)
        return chstring(this->usingStr_ + encoding::utf8_to_sysdcode(b), this->usingType_);
    else if (this->usingType_ == CStype::UTF8)
        return chstring(this->usingStr_ + encoding::sysdcode_to_utf8(b), this->usingType_);
}

// 迭代器相关方法
chstring::iterator chstring::begin( ) {
    return this->usingStr_.begin( );
}    // 返回起始迭代器
chstring::iterator chstring::end( ) {
    return this->usingStr_.end( );
}    // 返回结束迭代器
chstring::const_iterator chstring::begin( ) const {
    return this->usingStr_.begin( );
}    // 常量版本起始迭代器
chstring::const_iterator chstring::end( ) const {
    return this->usingStr_.end( );
}    // 常量版本结束迭代器
chstring::const_iterator chstring::cbegin( ) const {
    return this->usingStr_.cbegin( );
}    // 常量迭代器（C++11）
chstring::const_iterator chstring::cend( ) const {
    return this->usingStr_.cend( );
}    // 常量迭代器（C++11）

// 获取长度
size_t chstring::length( ) const {
    return this->usingStr_.length( );
}

// 检查是否为空
bool chstring::empty( ) const {
    return this->usingStr_.empty( );
}

// 获取大小
size_t chstring::size( ) const {
    return this->usingStr_.size( );
}

// 获取底层字符串
std::string chstring::get_u8string( ) const {
    if (usingType_ == CStype::UTF8)
        return this->usingStr_;
    else if (usingType_ == CStype::SYS)
        return encoding::sysdcode_to_utf8(this->usingStr_);
}

// 获取底层字符串，按照系统编码返回
std::string chstring::get_sysstring( ) const {
    if (usingType_ == CStype::UTF8)
        return encoding::utf8_to_sysdcode(this->usingStr_);
    else if (usingType_ == CStype::SYS)
        return this->usingStr_;
}

// 获取当前使用的文字的编码类型
chstring::CStype chstring::get_encoding_type( ) const {
    return this->usingType_;
}
