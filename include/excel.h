#pragma once

#ifndef EXCEL_H
#define EXCEL_H

#include <basic.hpp>
#include <chstring.hpp>
#include <iostream>
#include <xlnt/xlnt.hpp>

// 用于解析xlsx的类
namespace xlsx {

// 写入xlsx
class DefXlsxWrite {
public:
    DefXlsxWrite( )  = default;
    ~DefXlsxWrite( ) = default;

private:
};

// 解析xlsx
class DefXlsxLoad {
public:
    DefXlsxLoad( )  = default;
    ~DefXlsxLoad( ) = default;

private:
};


}    // namespace xlsx


#endif    // !EXCEL_H
