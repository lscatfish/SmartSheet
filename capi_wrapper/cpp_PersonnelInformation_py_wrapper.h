#ifndef CPP_PERSONNELINFORMATION_PY_WRAPPER_H
#define CPP_PERSONNELINFORMATION_PY_WRAPPER_H

#include <cpp_wrapper_header.h>

extern "C" {

// 默认构造对象的方式
__declspec(dllexport) PersonHanddle __cdecl DefPerson_createobj_default( );

// 关键信息构造
__declspec(dllexport) PersonHanddle __cdecl DefPerson_createobj_keyinformation(
    const char *_classname,
    const char *_name,
    const char *_studentID,
    const char *_academy,
    const char *_phonenumber);

__declspec(dllexport) bool __cdecl DefPerson_destroyobj(PersonHanddle);

__declspec(dllexport) void __cdecl DefPerson_set_classname(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_name(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_gender(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_grade(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_studentID(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_politicaloutlook(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_academy(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_majors(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_phonenumber(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_qqnumber(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_position(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_email(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_ethnicity(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_club(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_signPosition(PersonHanddle, const char *);
__declspec(dllexport) void __cdecl DefPerson_set_ifcheck(PersonHanddle, const bool);
__declspec(dllexport) void __cdecl DefPerson_set_ifsign(PersonHanddle, const bool);

DLL_EXPORT_C(const char *)
DefPerson_get_classname(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_name(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_gender(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_grade(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_studentID(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_politicaloutlook(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_academy(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_majors(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_phonenumber(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_qqnumber(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_position(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_email(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_ethnicity(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_club(PersonHanddle);
DLL_EXPORT_C(const char *)
DefPerson_get_signPosition(PersonHanddle);
DLL_EXPORT_C(const bool)
DefPerson_get_ifcheck(PersonHanddle);
DLL_EXPORT_C(const bool)
DefPerson_get_ifsign(PersonHanddle);

}

extern "C" {
DLL_EXPORT_C(PersonLineHanddle)
DefPersonLine_createobj_default( );


}



#endif    // !CPP_PERSONNELINFORMATION_PY_WRAPPER_H
