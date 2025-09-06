#ifndef CPP_WRAPPER_HEADER_H
#define CPP_WRAPPER_HEADER_H

#include <stdbool.h>
#include <type_traits>

//[@lscatfish：我为什么写这个宏？我的可以展开内联，写得很快]
#define CDECL_TYPE(RET_TYPE)        RET_TYPE __cdecl
#define DLL_EXPORT_C(RET_TYPE)      __declspec(dllexport) CDECL_TYPE(RET_TYPE)
#define CVT_PTR(inptr, outptr_type) (static_cast< outptr_type >(inptr))

//切勿定义一个0
#define BOOL_   1
#define CCHPTR_ 2

typedef const char *cchptr;
typedef void       *PersonHandle;
typedef void       *PersonLineHandle;

// 简化类型判断的模板（判断T是否为U类型）
template < typename T, typename U >
constexpr bool is_type = std::is_same_v< T, U >;


#endif    // !CPP_HEADER_H
