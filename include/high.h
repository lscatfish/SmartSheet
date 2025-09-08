#pragma once

#ifndef HIGH_H

#include <algorithm>
#include <cstdlib>
#include <type_traits>
#include <vector>

// 表格
template < typename _T >
using myTable = std::vector< std::vector< _T > >;    // 如果冲突，请封装到namespace中，或者改名为 MyTable

// 一行（列）
template < typename _T >
using myList = std::vector< _T >;    // 如果冲突，请封装到namespace中，或者改名为 MyList

// 模板函数：对 vector<T>按指定成员变量排序
// 参数：容器引用、比较函数（决定排序规则和依据的成员）
// @note [@lscatfish] ***多此一举***却似用筷子夹勺子喝粥***
template < typename _T, typename Compare >
void sort_my_list(myList< _T > &vec, Compare comp) {
    std::sort(vec.begin( ), vec.end( ), comp);
}
// @note [@lscatfish] ***多此一举***却似用筷子夹勺子喝粥***

#endif    // !HIGH_H
