#include <stdio.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <strsafe.h>

#else
#include <dlfcn.h>
#endif

#ifdef _WIN32

std::string _GetLastError()
{
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	std::string err((char*)lpMsgBuf);

	LocalFree(lpMsgBuf);
	return err;

}
#endif


void* _dlopen(const char *filename)
{
#ifdef _WIN32
	return (void*) LoadLibrary(filename);
#else
	return dlopen(filename, RTLD_LAZY);
#endif
}

std::string _dlerror(void)
{
#ifdef _WIN32
	return _GetLastError();
#else
	return dlerror();
#endif

}


void* _dlsym(void *handle, const char *symbol)
{
#ifdef _WIN32
	return GetProcAddress((HINSTANCE)handle, symbol);
#else
	return dlsym();
#endif
	
}

void _dlclose(void *handle)
{
#ifdef _WIN32
	::FreeLibrary((HINSTANCE)handle);
#else
	dlerror();
#endif
	
}