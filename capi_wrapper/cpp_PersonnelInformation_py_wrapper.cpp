
#include <cpp_PersonnelInformation_py_wrapper.h>
#include <PersonnelInformation.h>

#define OBJPTR_DefPerson CVT_PTR(obj, DefPerson *)

TCDECL(PersonHanddle)
DefPerson_createobj_default( ) {
    DefPerson *a = new (std::nothrow) DefPerson( );
    if (!a)
        return nullptr;
    return static_cast< PersonHanddle >(a);
}

TCDECL(PersonHanddle)
DefPerson_createobj_keyinformation(
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

TCDECL(bool)
DefPerson_destroyobj(PersonHanddle obj) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        delete (std::nothrow)a;
        return true;
    }
    return false;
}

TCDECL(void)
DefPerson_set_classname(PersonHanddle obj, const char *classname) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->classname = classname;
    }
}

TCDECL(void)
DefPerson_set_name(PersonHanddle obj, const char *name) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->name      = name;
    }
}

TCDECL(void)
DefPerson_set_gender(PersonHanddle obj, const char *gender) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->gender    = gender;
    }
}

TCDECL(void)
DefPerson_set_grade(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->grade     = cstr;
    }
}

TCDECL(void)
DefPerson_set_studentID(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->studentID = cstr;
    }
}

TCDECL(void)
DefPerson_set_politicaloutlook(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a        = OBJPTR_DefPerson;
        a->politicaloutlook = cstr;
    }
}

TCDECL(void)
DefPerson_set_academy(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->academy   = cstr;
    }
}

TCDECL(void)
DefPerson_set_majors(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->majors    = cstr;
    }
}

TCDECL(void)
DefPerson_set_phonenumber(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a   = OBJPTR_DefPerson;
        a->phonenumber = cstr;
    }
}

TCDECL(void)
DefPerson_set_qqnumber(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->qqnumber  = cstr;
    }
}

TCDECL(void)
DefPerson_set_position(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->position  = cstr;
    }
}

TCDECL(void)
DefPerson_set_email(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->email     = cstr;
    }
}

TCDECL(void)
DefPerson_set_ethnicity(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->ethnicity = cstr;
    }
}

TCDECL(void)
DefPerson_set_club(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->club      = cstr;
    }
}

TCDECL(void)
DefPerson_set_signPosition(PersonHanddle obj, const char *cstr) {
    if (obj) {
        DefPerson *a    = OBJPTR_DefPerson;
        a->signPosition = cstr;
    }
}

TCDECL(void)
DefPerson_set_ifcheck(PersonHanddle obj, const bool in) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->ifcheck   = in;
    }
}

TCDECL(void)
DefPerson_set_ifsign(PersonHanddle obj, const bool in) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        a->ifsign    = in;
    }
}

TCDECL(const char *)
DefPerson_get_classname(PersonHanddle obj) {
    if (obj) {
        DefPerson *a = OBJPTR_DefPerson;
        return a->classname.c_str( );
    }
    return nullptr
}
