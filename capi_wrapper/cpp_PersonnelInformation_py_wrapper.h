#ifndef CPP_PERSONNELINFORMATION_PY_WRAPPER_H
#define CPP_PERSONNELINFORMATION_PY_WRAPPER_H

#include <cpp_wrapper_header.h>
#include <PersonnelInformation.h>

#define OBJPTR_DefPerson CVT_PTR(obj, DefPerson *)
#define OBJPTR_DefLine   CVT_PTR(obj, DefLine *)

// 声明函数
#define STATE_SETTER_PERSON(def_type, attr, attr_type) \
    DLL_EXPORT_C(def_type)                             \
    DefPerson_set_##attr(PersonHandle, attr_type);

// 定义函数
#define DEF_SETTER_PERSON(def_type, attr, attr_type)            \
    CDECL_TYPE(def_type)                                        \
    DefPerson_set_##attr(PersonHandle obj, attr_type _##attr) { \
        if (obj) {                                              \
            DefPerson *a = OBJPTR_DefPerson;                    \
            a->attr      = _##attr;                             \
        }                                                       \
    }

#define STATE_GETTER_PERSON(def_type, attr) \
    DLL_EXPORT_C(def_type)                  \
    DefPerson_get_##attr(PersonHandle);

#define DEF_GETTER_PERSON_CCHPTR(attr)       \
    CDECL_TYPE(cchptr)                       \
    DefPerson_get_##attr(PersonHandle obj) { \
        if (obj) {                           \
            DefPerson *a = OBJPTR_DefPerson; \
            return a->attr.c_str( );         \
        }                                    \
        return nullptr;                      \
    }

#define DEF_GETTER_PERSON_BOOL(attr)         \
    CDECL_TYPE(bool)                         \
    DefPerson_get_##attr(PersonHandle obj) { \
        if (obj) {                           \
            DefPerson *a = OBJPTR_DefPerson; \
            return a->attr;                  \
        }                                    \
        return false;                        \
    }



extern "C" {

// 默认构造对象的方式
DLL_EXPORT_C(PersonHandle)
create_DefPerson_default( );

// 关键信息构造
DLL_EXPORT_C(PersonHandle)
create_DefPerson_keyinformation(
    const char *_classname,
    const char *_name,
    const char *_studentID,
    const char *_academy,
    const char *_phonenumber);

DLL_EXPORT_C(bool)
destroy_DefPerson(PersonHandle);

STATE_SETTER_PERSON(void, classname, cchptr)
STATE_SETTER_PERSON(void, name, cchptr)
STATE_SETTER_PERSON(void, gender, cchptr)
STATE_SETTER_PERSON(void, grade, cchptr)
STATE_SETTER_PERSON(void, studentID, cchptr)
STATE_SETTER_PERSON(void, politicaloutlook, cchptr)
STATE_SETTER_PERSON(void, academy, cchptr)
STATE_SETTER_PERSON(void, majors, cchptr)
STATE_SETTER_PERSON(void, phonenumber, cchptr)
STATE_SETTER_PERSON(void, qqnumber, cchptr)
STATE_SETTER_PERSON(void, position, cchptr)
STATE_SETTER_PERSON(void, email, cchptr)
STATE_SETTER_PERSON(void, ethnicity, cchptr)
STATE_SETTER_PERSON(void, club, cchptr)
STATE_SETTER_PERSON(void, signPosition, cchptr)
STATE_SETTER_PERSON(void, ifcheck, bool)
STATE_SETTER_PERSON(void, ifsign, bool)
STATE_GETTER_PERSON(cchptr, classname)
STATE_GETTER_PERSON(cchptr, name)
STATE_GETTER_PERSON(cchptr, gender)
STATE_GETTER_PERSON(cchptr, grade)
STATE_GETTER_PERSON(cchptr, studentID)
STATE_GETTER_PERSON(cchptr, politicaloutlook)
STATE_GETTER_PERSON(cchptr, academy)
STATE_GETTER_PERSON(cchptr, majors)
STATE_GETTER_PERSON(cchptr, phonenumber)
STATE_GETTER_PERSON(cchptr, qqnumber)
STATE_GETTER_PERSON(cchptr, position)
STATE_GETTER_PERSON(cchptr, email)
STATE_GETTER_PERSON(cchptr, ethnicity)
STATE_GETTER_PERSON(cchptr, club)
STATE_GETTER_PERSON(cchptr, signPosition)
STATE_GETTER_PERSON(bool, ifcheck)
STATE_GETTER_PERSON(bool, ifsign)

DLL_EXPORT_C(void)
DefPerson_set_otherInformation(PersonHandle, cchptr, cchptr);
DLL_EXPORT_C(cchptr)
DefPerson_get_otherInformation(PersonHandle, cchptr);
DLL_EXPORT_C(bool)
DefPerson_findkey_otherInformation(PersonHandle, cchptr);
DLL_EXPORT_C(bool)
DefPerson_erasekey_otherInformation(PersonHandle, cchptr);

DLL_EXPORT_C(void)
DefPerson_optimize(PersonHandle);
}




extern "C" {
DLL_EXPORT_C(PersonLineHandle)
create_DefLine_default( );

DLL_EXPORT_C(PersonLineHandle)
create_DefLine_keyinformation(
    const char *_classname,
    const char *_name,
    const char *_studentID,
    const char *_academy,
    const char *_phonenumber);

DLL_EXPORT_C(bool)
destroy_DefLine(PersonLineHandle);

DLL_EXPORT_C(void)
DefLine_set_classname(PersonLineHandle, cchptr);
DLL_EXPORT_C(void)
DefLine_set_ifcheck(PersonLineHandle, bool);
DLL_EXPORT_C(void)
DefLine_set_ifsign(PersonLineHandle, bool);
DLL_EXPORT_C(cchptr)
DefLine_get_classname(PersonLineHandle);
DLL_EXPORT_C(bool)
DefLine_get_ifcheck(PersonLineHandle);
DLL_EXPORT_C(bool)
DefLine_get_ifsign(PersonLineHandle);

DLL_EXPORT_C(void)
DefLine_set_information(PersonLineHandle, cchptr, cchptr);
DLL_EXPORT_C(cchptr)
DefLine_get_information(PersonLineHandle, cchptr);
DLL_EXPORT_C(bool)
DefLine_findkey_information(PersonLineHandle, cchptr);
DLL_EXPORT_C(bool)
DefLine_erasekey_information(PersonLineHandle, cchptr);
}



#endif    // !CPP_PERSONNELINFORMATION_PY_WRAPPER_H
