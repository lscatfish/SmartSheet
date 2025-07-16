#pragma once

#ifndef QINGZICLASS_H
#define QINGZICLASS_H

#include "ChineseEncoding.h"
#include "Files.h"
#include "PersonnelInformation.h"
#include <string>
#include <vector>


// 操作青字班的类
class DoQingziClass {
public:
    DoQingziClass( );
    ~DoQingziClass( );
    void start( );//主控函数

private:
    std::vector< DefStdPerson >   personStd;
    std::vector< DefUnstdPerson > personUnstd;
};



#endif    // !QINGZICLASS_H
