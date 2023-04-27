#ifndef QEXCEL_GLOBAL_H
#define QEXCEL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QEXCEL_LIBRARY)
#  define QEXCELSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QEXCELSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QEXCEL_GLOBAL_H
