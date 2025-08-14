
/* =================================================================================
 * @brief 用于操作中文编码的文件
 * @note 没有从string派生是因为string没有虚析构函数，因此通过基类指针删除派生类对象可能导致未定义行为
 * 作者：   刘思成
 * 邮箱：   2561925435@qq.com
 * ================================================================================= */

#pragma once

#ifndef CHSTRING_HPP
#define CHSTRING_HPP

#include <Encoding.h>
#include <iostream>
#include <string>

class chstring {
public:
    std::string str;

    chstring(std::string _in_ );
    ~chstring( );

private:
};

chstring::chstring(std::string _in_) {
    this->str = _in_;
}

chstring::~chstring( ) {
}


#endif    // !CHSTRING_HPP
