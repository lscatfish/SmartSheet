
#include <cpp_PersonnelInformation_py_wrapper.h>
#include <PersonnelInformation.h>



CDECL_TYPE(PersonHanddle)
DefPerson_create_default( ) {
    DefPerson *a = new (std::nothrow) DefPerson( );
    if (!a)
        return nullptr;
    return static_cast< PersonHanddle >(a);
}

CDECL_TYPE(PersonHanddle)
DefPerson_create_keyinformation(
    const char *_classname,
    const char *_name,
    const char *_studentID,
    const char *_academy,
    const char *_phonenumber) {
    DefPerson *a = new (std::nothrow) DefPerson(_classname, _name, _studentID, _academy, _phonenumber);
    if (!a)
        return nullptr;
    return static_cast< PersonHanddle >(a);
}

CDECL_TYPE(bool)
DefPerson_destroyobj(PersonHanddle obj) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        delete (std::nothrow)a;
        return true;
    }
    return false;
}

DEF_SETTER_PERSON(void, classname, cchptr)
DEF_SETTER_PERSON(void, name, cchptr)
DEF_SETTER_PERSON(void, gender, cchptr)
DEF_SETTER_PERSON(void, grade, cchptr)
DEF_SETTER_PERSON(void, studentID, cchptr)
DEF_SETTER_PERSON(void, politicaloutlook, cchptr)
DEF_SETTER_PERSON(void, academy, cchptr)
DEF_SETTER_PERSON(void, majors, cchptr)
DEF_SETTER_PERSON(void, phonenumber, cchptr)
DEF_SETTER_PERSON(void, qqnumber, cchptr)
DEF_SETTER_PERSON(void, position, cchptr)
DEF_SETTER_PERSON(void, email, cchptr)
DEF_SETTER_PERSON(void, ethnicity, cchptr)
DEF_SETTER_PERSON(void, club, cchptr)
DEF_SETTER_PERSON(void, signPosition, cchptr)
DEF_SETTER_PERSON(void, ifcheck, bool)
DEF_SETTER_PERSON(void, ifsign, bool)

DEF_GETTER_PERSON(cchptr, classname)
DEF_GETTER_PERSON(cchptr, name)
DEF_GETTER_PERSON(cchptr, gender)
DEF_GETTER_PERSON(cchptr, grade)
DEF_GETTER_PERSON(cchptr, studentID)
DEF_GETTER_PERSON(cchptr, politicaloutlook)
DEF_GETTER_PERSON(cchptr, academy)
DEF_GETTER_PERSON(cchptr, majors)
DEF_GETTER_PERSON(cchptr, phonenumber)
DEF_GETTER_PERSON(cchptr, qqnumber)
DEF_GETTER_PERSON(cchptr, position)
DEF_GETTER_PERSON(cchptr, email)
DEF_GETTER_PERSON(cchptr, ethnicity)
DEF_GETTER_PERSON(cchptr, club)
DEF_GETTER_PERSON(cchptr, signPosition)
DEF_GETTER_PERSON(bool, ifcheck)
DEF_GETTER_PERSON(bool, ifsign)

CDECL_TYPE(void)
DefPerson_set_otherInformation(PersonHanddle obj, cchptr key, cchptr in) {
    if (obj) {
        DefPerson *a             = OBJPTR_DefPerson;
        a->otherInformation[key] = in;
    }
}

CDECL_TYPE(cchptr)
DefPerson_get_otherInformation(PersonHanddle obj, cchptr key) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        if (a->otherInformation.find(key) != a->otherInformation.end( )) {
            return a->otherInformation[key].c_str( );
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

CDECL_TYPE(bool)
DefPerson_findkey_otherInformation(PersonHanddle obj, cchptr key) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        if (a->otherInformation.find(key) != a->otherInformation.end( )) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

CDECL_TYPE(bool)
DefPerson_erasekey_otherInformation(PersonHanddle obj, cchptr key) {
    if (obj) {
        DefPerson *a  = OBJPTR_DefPerson;
        auto       it = a->otherInformation.find(key);
        if (it != a->otherInformation.end( )) {
            a->otherInformation.erase(it);
            return true;
        }
        return false;
    }
    return false;
}
