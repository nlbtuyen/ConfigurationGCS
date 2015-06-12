#ifndef QEXTSERIALPORT_GLOBAL
#define QEXTSERIALPORT_GLOBAL

#include <QtCore/QtGlobal>

#ifdef QEXTSERIALPORT_BUILD_SHARED
#  define QEXTSERIALPORT_EXPORT Q_DECL_EXPORT
#elif defined(QEXTSERIALPORT_USING_SHARED)
#  define QEXTSERIALPORT_EXPORT Q_DECL_IMPORT
#else
#  define QEXTSERIALPORT_EXPORT
#endif

// ### for compatible with old version. should be removed in QESP 2.0
#ifdef _TTY_NOWARN_
#  define QESP_NO_WARN
#endif
#ifdef _TTY_NOWARN_PORT_
#  define QESP_NO_PORTABILITY_WARN
#endif

/*if all warning messages are turned off, flag portability warnings to be turned off as well*/
#ifdef QESP_NO_WARN
#  define QESP_NO_PORTABILITY_WARN
#endif

/*macros for warning and debug messages*/
#ifdef QESP_NO_PORTABILITY_WARN
#  define QESP_PORTABILITY_WARNING  while (false)qWarning
#else
#  define QESP_PORTABILITY_WARNING qWarning
#endif /*QESP_NOWARN_PORT*/

#ifdef QESP_NO_WARN
#  define QESP_WARNING while (false)qWarning
#else
#  define QESP_WARNING qWarning
#endif /*QESP_NOWARN*/

#endif // QEXTSERIALPORT_GLOBAL

