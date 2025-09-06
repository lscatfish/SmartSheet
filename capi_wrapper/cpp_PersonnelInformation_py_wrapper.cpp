
#include <cpp_PersonnelInformation_py_wrapper.h>
#include <PersonnelInformation.h>


CDECL_TYPE(PersonHandle)
DefPerson_create_default( ) {
    DefPerson *a = new (std::nothrow) DefPerson( );
    if (!a)
        return nullptr;
    return static_cast< PersonHandle >(a);
}

CDECL_TYPE(PersonHandle)
DefPerson_create_keyinformation(
    const char *_classname,
    const char *_name,
    const char *_studentID,
    const char *_academy,
    const char *_phonenumber) {
    DefPerson *a = new (std::nothrow) DefPerson(_classname, _name, _studentID, _academy, _phonenumber);
    if (!a)
        return nullptr;
    return static_cast< PersonHandle >(a);
}

CDECL_TYPE(bool)
DefPerson_destroy(PersonHandle obj) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        delete a;
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

DEF_GETTER_PERSON_CCHPTR(classname)
DEF_GETTER_PERSON_CCHPTR(name)
DEF_GETTER_PERSON_CCHPTR(gender)
DEF_GETTER_PERSON_CCHPTR(grade)
DEF_GETTER_PERSON_CCHPTR(studentID)
DEF_GETTER_PERSON_CCHPTR(politicaloutlook)
DEF_GETTER_PERSON_CCHPTR(academy)
DEF_GETTER_PERSON_CCHPTR(majors)
DEF_GETTER_PERSON_CCHPTR(phonenumber)
DEF_GETTER_PERSON_CCHPTR(qqnumber)
DEF_GETTER_PERSON_CCHPTR(position)
DEF_GETTER_PERSON_CCHPTR(email)
DEF_GETTER_PERSON_CCHPTR(ethnicity)
DEF_GETTER_PERSON_CCHPTR(club)
DEF_GETTER_PERSON_CCHPTR(signPosition)
DEF_GETTER_PERSON_BOOL(ifcheck)
DEF_GETTER_PERSON_BOOL(ifsign)

CDECL_TYPE(void)
DefPerson_set_otherInformation(PersonHandle obj, cchptr key, cchptr in) {
    if (obj) {
        DefPerson *a             = OBJPTR_DefPerson;
        a->otherInformation[key] = in;
    }
}

CDECL_TYPE(cchptr)
DefPerson_get_otherInformation(PersonHandle obj, cchptr key) {
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
DefPerson_findkey_otherInformation(PersonHandle obj, cchptr key) {
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
DefPerson_erasekey_otherInformation(PersonHandle obj, cchptr key) {
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







CDECL_TYPE(PersonLineHandle)
DefLine_create_default( ) {
    DefLine *obj = new (std::nothrow) DefLine( );
    if (obj) {
        PersonLineHandle a = CVT_PTR(obj, PersonLineHandle);
        return a;
    }
    return nullptr;
}

CDECL_TYPE(PersonLineHandle)
DefLine_create_keyinformation(
    const char *_classname,
    const char *_name,
    const char *_studentID,
    const char *_academy,
    const char *_phonenumber) {
    DefLine *obj = new (std::nothrow) DefLine(_classname, _name, _studentID, _academy, _phonenumber);
    if (obj) {
        return CVT_PTR(obj, PersonLineHandle);
    }
    return nullptr;
}

CDECL_TYPE(void)
DefLine_set_classname(PersonLineHandle obj, cchptr _classname) {
    if (obj) {
        DefLine *a   = OBJPTR_DefLine;
        a->classname = _classname;
    }
}

CDECL_TYPE(void)
DefLine_set_ifcheck(PersonLineHandle obj, bool _ifcheck) {
    if (obj) {
        DefLine *a = OBJPTR_DefLine;
        a->ifcheck = _ifcheck;
    }
}

CDECL_TYPE(void)
DefLine_set_ifsign(PersonLineHandle obj, bool _ifsign) {
    if (obj) {
        DefLine *a = OBJPTR_DefLine;
        a->ifsign  = _ifsign;
    }
}

CDECL_TYPE(cchptr)
DefLine_get_classname(PersonLineHandle obj) {
    if (obj) {
        DefLine *a = OBJPTR_DefLine;
        return a->classname.c_str( );
    }
    return nullptr;
}

CDECL_TYPE(bool)
DefLine_get_ifcheck(PersonLineHandle obj) {
    if (obj) {
        DefLine *a = OBJPTR_DefLine;
        return a->ifcheck;
    }
    return false;
}

CDECL_TYPE(bool)
DefLine_get_ifsign(PersonLineHandle obj) {
    if (obj) {
        DefLine *a = OBJPTR_DefLine;
        return a->ifsign;
    }
    return false;
}

CDECL_TYPE(void)
DefLine_set_information(PersonHandle obj, cchptr key, cchptr in) {
    if (obj) {
        DefLine *a          = OBJPTR_DefLine;
        a->information[key] = in;
    }
}

CDECL_TYPE(cchptr)
DefLine_get_information(PersonHandle obj, cchptr key) {
    if (obj) {
        DefLine *a = OBJPTR_DefLine;
        if (a->information.find(key) != a->information.end( )) {
            return a->information[key].c_str( );
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

CDECL_TYPE(bool)
DefLine_findkey_information(PersonHandle obj, cchptr key) {
    if (obj) {
        DefLine *a = OBJPTR_DefLine;
        if (a->information.find(key) != a->information.end( )) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

CDECL_TYPE(bool)
DefLine_erasekey_information(PersonHandle obj, cchptr key) {
    if (obj) {
        DefLine *a  = OBJPTR_DefLine;
        auto     it = a->information.find(key);
        if (it != a->information.end( )) {
            a->information.erase(it);
            return true;
        }
        return false;
    }
    return false;
}
