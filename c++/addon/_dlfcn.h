#ifndef _NODEPLUGIN_DYNAMIC_LIB_WRAP_H_
#define _NODEPLUGIN_DYNAMIC_LIB_WRAP_H_
#include <string>
void* _dlopen(const char *filename);
std::string _dlerror(void);
void* _dlsym(void *handle, const char *symbol);
void   _dlclose(void *handle);
#endif