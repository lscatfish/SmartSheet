/* =================================================================================
 * @file    chstring.cpp
 *
 * @attention [2025.08.29][@lscatfish]此hpp尚未集成到项目中
 *
 * @brief 用于操作中文编码的文件
 * @note 没有从string派生是因为string没有虚析构函数，因此通过基类指针删除派生类对象可能导致未定义行为
 * -------250826----------$$$@note 希望此类可以集成到icu，以达到对中文编码的精确识别$$$---------------
 * @note [2025.08.26][@lscatfish]已集成icu
 *
 * @todo [2025.08.26][@lscatfish]希望将此hpp集成到项目中，所有的字符串都由此类实现
 *
 * 作者：lscatfish
 * 邮箱：2561925435@qq.com
 * ================================================================================= */

#include <algorithm>
#include <basic.hpp>
#include <cctype>
#include <chstring.hpp>
#include <Encoding.h>
#include <helper.h>
#include <iostream>
#include <string>
#include <utility>


// 交换a，b的内容
void chstring::swap(chstring &a, chstring &b) {
    std::swap(a.usingStr_, b.usingStr_);
    std::swap(a.usingType_, b.usingType_);
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
    if (this->usingType_ == csType::SYS)
        return chstring(this->usingStr_ + encoding::utf8_to_sysdcode(b), this->usingType_);
    else if (this->usingType_ == csType::UTF8)
        return chstring(this->usingStr_ + encoding::sysdcode_to_utf8(b), this->usingType_);
    else
        return chstring( );
}

// 字符串拼接实现（修正返回类型为chstring）(总是匹配到前一个字符的编码形式)
chstring chstring::operator+(const char *b) const {
    if (this->usingType_ == csType::SYS)
        return chstring(this->usingStr_ + encoding::utf8_to_sysdcode(b), this->usingType_);
    else if (this->usingType_ == csType::UTF8)
        return chstring(this->usingStr_ + encoding::sysdcode_to_utf8(b), this->usingType_);
    else
        return chstring( );
}

// 字符串拼接实现（修正返回类型为chstring的引用）(总是匹配到前一个字符的编码形式)
chstring &chstring::operator+=(const chstring &b) {
    if (this->usingType_ == b.usingType_) {
        this->usingStr_ += b.usingStr_;
    } else {
        chstring toMain(b, this->usingType_);
        this->usingStr_ += toMain.usingStr_;
    }
    return *this;
}

// 字符串拼接实现（修正返回类型为chstring的引用）(总是匹配到前一个字符的编码形式)
chstring &chstring::operator+=(const std::string &b) {
    if (this->usingType_ == csType::SYS)
        this->usingStr_ += encoding::utf8_to_sysdcode(b);
    else if (this->usingType_ == csType::UTF8)
        this->usingStr_ += encoding::sysdcode_to_utf8(b);
    return *this;
}

// 字符串拼接实现（修正返回类型为chstring的引用）(总是匹配到前一个字符的编码形式)
chstring &chstring::operator+=(const char *b) {
    *this += std::string(b);
    return *this;
}


// 迭代器相关方法
chstring::iterator chstring::begin( ) {
    return this->usingStr_.begin( );
}    // 返回起始迭代器
chstring::iterator chstring::end( ) {
    return this->usingStr_.end( );
}    // 返回结束迭代器
chstring::reverse_iterator chstring::rbegin( ) {
    return this->usingStr_.rbegin( );
}
chstring::reverse_iterator chstring::rend( ) {
    return this->usingStr_.rend( );
}
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

// 删除最后一位
void chstring::pop_back( ) {
    this->usingStr_.pop_back( );
}

// 打印文字到控制台
void chstring::print( ) const {
    std::cout << *this;
}

// 打赢文字与回车到控制台
void chstring::println( ) const {
    std::cout << *this << std::endl;
}

// 获取底层字符串
std::string chstring::u8string( ) const {
    if (this->usingType_ == csType::UTF8)
        return this->usingStr_;
    else if (this->usingType_ == csType::SYS)
        return encoding::sysdcode_to_utf8(this->usingStr_);
    else
        return "";
}

// 获取底层字符串，按照系统编码返回
std::string chstring::sysstring( ) const {
    if (this->usingType_ == csType::UTF8)
        return encoding::utf8_to_sysdcode(this->usingStr_);
    else if (this->usingType_ == csType::SYS)
        return this->usingStr_;
    else
        return "";
}

// 获取底层字符串，按照当前编码返回
std::string chstring::usstring( ) const {
    return this->usingStr_;
}

// 获取宽字符方式
std::wstring chstring::wstring( ) const {
    if (this->usingType_ == csType::UTF8)
        return encoding::utf8_to_wstring(this->usingStr_);
    else if (this->usingType_ == csType::SYS)
        return encoding::utf8_to_wstring(encoding::sysdcode_to_utf8(this->usingStr_));
    else
        return L"";
}

// 获取当前使用的文字的编码类型
chstring::csType chstring::cstype( ) const {
    return this->usingType_;
}

// 转化编码
void chstring::cvtEncode_to(csType _in) {
    if (_in == this->usingType_)
        return;
    else if (_in == csType::SYS)
        this->usingStr_ = encoding::utf8_to_sysdcode(this->usingStr_);
    else if (_in == csType::UTF8)
        this->usingStr_ = encoding::sysdcode_to_utf8(this->usingStr_);
}

// 清除字符串前后的所有空白字符（包括空格、\t、\n等）
void chstring::trim_whitespace( ) {
    this->usingStr_ = ::trim_whitespace(this->usingStr_);
    if (this->usingStr_.size( ) == 0) {
        this->usingType_ = csType::UTF8;
        this->usingStr_  = "";
        return;
    } else {
        return;
    }
}

// 分离中文与数字
std::pair< chstring, chstring > chstring::split_chinese_and_number( ) const {
    auto result = ::split_chinese_and_number(this->usingStr_);
    return std::pair< chstring, chstring >(chstring(result.first, this->usingType_), chstring(result.second, this->usingType_));
}

// 分离键与值
std::pair< chstring, chstring > chstring::split_by_equal( ) const {
    auto r = ::split_by_equal(this->usingStr_);
    return std::pair< chstring, chstring >(chstring(r.first, this->usingType_), chstring(r.second, this->usingType_));
}

// 按照第一个char分离
std::pair< chstring, chstring > chstring::split_by_first_of(const char cutter) const {
    auto [first, last] = ::split_by_first_of(this->usingStr_, cutter);
    return std::pair< chstring, chstring >(chstring(first, this->usingType_), chstring(last, this->usingType_));
}

// 按照最后一个char分离
std::pair< chstring, chstring > chstring::split_by_last_of(const char cutter) const {
    auto [first, last] = ::split_by_last_of(this->usingStr_, cutter);
    return std::pair< chstring, chstring >(chstring(first, this->usingType_), chstring(last, this->usingType_));
}

// 按照第一个char分离
std::pair< chstring, chstring > chstring::split_by_first_of(const std::string &cutter) const {
    auto [first, last] = ::split_by_first_of(this->usingStr_, cutter);
    return std::pair< chstring, chstring >(chstring(first, this->usingType_), chstring(last, this->usingType_));
}

// 按照最后一个char分离
std::pair< chstring, chstring > chstring::split_by_last_of(const std::string &cutter) const {
    auto [first, last] = ::split_by_last_of(this->usingStr_, cutter);
    return std::pair< chstring, chstring >(chstring(first, this->usingType_), chstring(last, this->usingType_));
}

// 是否都是数字
bool chstring::is_all_digits( ) const {
    if (this->usingStr_.empty( )) return false;
    return std::all_of(this->usingStr_.begin( ), this->usingStr_.end( ), [](unsigned char c) { return std::isdigit(c); });
}
