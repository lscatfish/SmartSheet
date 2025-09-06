
#include <basic.hpp>
#include <cpp_QingziClass_py_wrapper.h>
#include <helper.h>
#include <iostream>
#include <new>
#include <QingziClass.h>


CDECL_TYPE(QingziClassHandle)
create_DoQingziClass( ) {
    DoQingziClass *obj = new (std::nothrow) DoQingziClass;
    if (!obj) return nullptr;
    if (!obj->self_check( )) {
        std::cout << std::endl;
        std::cerr << U8C(u8"青字班模块自检失败，程序终止...") << std::endl;
        delete obj;
        pause( );
        return nullptr;
    }
    return CVT_PTR(obj, QingziClassHandle);
}

CDECL_TYPE(bool)
destroy_DoQingziClass(QingziClassHandle obj) {
    if (obj) {
        DoQingziClass *a = OBJPTR_DoQingziClass;
        delete a;
        return true;
    }
    return false;
}
