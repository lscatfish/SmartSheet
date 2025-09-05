#ifndef CPP_PERSONNELINFORMATION_PY_WRAPPER_H
#define CPP_PERSONNELINFORMATION_PY_WRAPPER_H

#include <cpp_wrapper_header.h>

#define OBJPTR_DefPerson CVT_PTR(obj, DefPerson *)

// 声明函数
#define STATE_SETTER_PERSON(def_type, attr, attr_type) \
    DLL_EXPORT_C(def_type)                             \
    DefPerson_set_##attr(PersonHanddle, attr_type);

// 定义函数
#define DEF_SETTER_PERSON(def_type, attr, attr_type)             \
    CDECL_TYPE(def_type)                                         \
    DefPerson_set_##attr(PersonHanddle obj, attr_type _##attr) { \
        if (obj) {                                               \
            DefPerson *a = OBJPTR_DefPerson;                     \
            a->attr      = _##attr;                              \
        }                                                        \
    }

#define STATE_GETTER_PERSON(def_type, attr) \
    DLL_EXPORT_C(def_type)                  \
    DefPerson_get_##attr(PersonHanddle);

#define DEF_GETTER_PERSON(def_type, attr)                   \
    CDECL_TYPE(def_type)                                    \
    DefPerson_get_##attr(PersonHanddle obj) {               \
        if (obj) {                                          \
            DefPerson *a = OBJPTR_DefPerson;                \
            if constexpr (is_type< def_type, bool >)        \
                return a->attr;                             \
            else if constexpr (is_type< def_type, cchptr >) \
                return a->attr.c_str( );                    \
        }                                                   \
        if constexpr (is_type< def_type, bool >)            \
            return false;                                   \
        else if constexpr (is_type< def_type, cchptr >)     \
            return nullptr;                                 \
    }

extern "C" {

// 默认构造对象的方式
DLL_EXPORT_C(PersonHanddle)
DefPerson_create_default( );

// 关键信息构造
DLL_EXPORT_C(PersonHanddle)
DefPerson_create_keyinformation(
    const char *_classname,
    const char *_name,
    const char *_studentID,
    const char *_academy,
    const char *_phonenumber);

DLL_EXPORT_C(bool)
DefPerson_destroyobj(PersonHanddle);

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
DefPerson_set_otherInformation(PersonHanddle, cchptr, cchptr);

DLL_EXPORT_C(cchptr)
DefPerson_get_otherInformation(PersonHanddle, cchptr);

DLL_EXPORT_C(bool)
DefPerson_findkey_otherInformation(PersonHanddle, cchptr);

DLL_EXPORT_C(bool)
DefPerson_erasekey_otherInformation(PersonHanddle, cchptr);

}




extern "C" {
DLL_EXPORT_C(PersonLineHanddle)
DefPersonLine_createobj_default( );
}



#endif    // !CPP_PERSONNELINFORMATION_PY_WRAPPER_H
