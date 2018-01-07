#include "plugin.h"

#ifdef _WIN32
#include <windows.h>
#include <strsafe.h>

#else
#include <dlfcn.h>
#endif

struct plugin_t {

#ifdef _WIN32
	HINSTANCE handle;
#else
	void*     handle;
#endif
	char error[128];

	void  (*plugin_call)(void* plug, plugin_param_t* in, plugin_callback_t cb);

};
typedef struct plugin_t plugin_t;

#ifdef _WIN32

void _GetLastError(plugin_t* plugin) {
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

	strncpy(plugin->error, (char*)lpMsgBuf, sizeof(plugin->error) - 1);

	LocalFree(lpMsgBuf);

}

#endif

#define PLUG_CLOSE( plug) plug_close(plug);plug=NULL;

void* plugin_open(const char* path, plugin_param_t* err) {
	plugin_t* plug = malloc(sizeof(plugin_t));
	memset(plug, 0, sizeof(plugin_t));

#ifdef _WIN32
	plug->handle = LoadLibrary(path);
	if (plug->handle == NULL) {
		_GetLastError(plug);
		if (err) {
			strncpy(err->data, plug->error, err->size);
		}
		free(plug);
		return NULL;
	}
	typedef void(*FUNCADDR)();
	plug->plugin_call = (FUNCADDR)(GetProcAddress(plug->handle, "plug_call"));
	if (!plug->plugin_call) {
		if (err) {
			strncpy(err->data, "no 'plug_call' symbol in plug",err->size);
		}
		PLUG_CLOSE(plug);
		return NULL;
	}


#else
#endif
	return plug;
}


void plug_close(void* plug) {
	if (plug) {
		FreeLibrary(plug);
		free(plug);
	}
}

void  plug_call(void* plug, const char* buf, int size, plug_callback_t returns) {
		((plug_t*)plug)->plug_call(buf, size, returns);
}
