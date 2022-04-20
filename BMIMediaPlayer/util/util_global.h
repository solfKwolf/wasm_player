#ifndef UTIL_GLOBAL_H
#define UTIL_GLOBAL_H

#ifdef __WIN32__
#if defined(UTIL_LIBRARY)
#  define UTILSHARED_EXPORT __declspec(dllexport)
#else
#  define UTILSHARED_EXPORT __declspec(dllimport)
#endif
#else
#  define UTILSHARED_EXPORT
#endif

#endif // UTIL_GLOBAL_H
