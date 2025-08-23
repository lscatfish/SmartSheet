
#pragma once

#ifndef BASIC_HPP
#define BASIC_HPP

#include <type_traits>
#include <vector>

// 表格
template < typename _T >
using table = std::vector< std::vector< _T > >;

// 一行（列）
template < typename _T >
using list = std::vector< _T >;

#define U8C(s) reinterpret_cast< const char * >(s)

#endif    // !BASIC_H
