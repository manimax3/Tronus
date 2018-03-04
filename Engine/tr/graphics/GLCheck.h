#pragma once
#include <tr.h>

#ifdef TR_DEBUG
#define Call(...)                                                              \
    __VA_ARGS__;                                                               \
    GLCheck(__FILE__, __LINE__, #__VA_ARGS__);
#else
#define Call(...) __VA_ARGS__;
#endif

void GLCheck(const char *file, int line, const char *functions);
