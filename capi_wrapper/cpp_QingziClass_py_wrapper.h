
#ifndef CPP_QINGZICLASS_PY_WRAPPER_H
#define CPP_QINGZICLASS_PY_WRAPPER_H

#include <cpp_wrapper_header.h>
#include <QingziClass.h>

#define OBJPTR_DoQingziClass CVT_PTR(obj, DoQingziClass *)


DLL_EXPORT_C(QingziClassHandle)
create_DoQingziClass( );

DLL_EXPORT_C(bool)
destroy_DoQingziClass(QingziClassHandle);



#endif    // !CPP_QINGZICLASS_PY_WRAPPER_H
