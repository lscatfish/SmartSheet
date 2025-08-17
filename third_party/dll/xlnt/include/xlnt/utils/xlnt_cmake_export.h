
#ifndef XLNT_API_H
#define XLNT_API_H

#ifdef XLNT_CMAKE_STATIC_DEFINE
#  define XLNT_API
#  define XLNT_CMAKE_NO_EXPORT
#else
#  ifndef XLNT_API
#    ifdef xlnt_EXPORTS
        /* We are building this library */
#      define XLNT_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define XLNT_API __declspec(dllimport)
#    endif
#  endif

#  ifndef XLNT_CMAKE_NO_EXPORT
#    define XLNT_CMAKE_NO_EXPORT 
#  endif
#endif

#ifndef XLNT_DEPRECATED
#  define XLNT_DEPRECATED __declspec(deprecated)
#endif

#ifndef XLNT_DEPRECATED_EXPORT
#  define XLNT_DEPRECATED_EXPORT XLNT_API XLNT_DEPRECATED
#endif

#ifndef XLNT_DEPRECATED_NO_EXPORT
#  define XLNT_DEPRECATED_NO_EXPORT XLNT_CMAKE_NO_EXPORT XLNT_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef XLNT_CMAKE_NO_DEPRECATED
#    define XLNT_CMAKE_NO_DEPRECATED
#  endif
#endif

#endif /* XLNT_API_H */
