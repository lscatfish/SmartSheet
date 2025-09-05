#ifndef CPP_WRAPPER_HEADER_H
#define CPP_WRAPPER_HEADER_H


//[@lscatfish：我为什么写这个宏？我的可以展开内联，写得很快]
#define DLL_EXPORT_C(RET_TYPE)      __declspec(dllexport) RET_TYPE __cdecl
#define TCDECL(RET_TYPE)            RET_TYPE __cdecl
#define OUTPTR_TYPE( )
#define CVT_PTR(inptr, outptr_type) (static_cast< outptr_type >(inptr))

typedef void *PersonHanddle;
typedef void *PersonLineHanddle;

#endif    // !CPP_HEADER_H
