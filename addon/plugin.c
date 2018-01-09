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

	void (*plugin_call)(void* plug, plugin_param_t* in, void* context,
		void(*cb)(void* context, plugin_param_t* param));

	void (*plugin_set_notification)(void* plugin, void* context,
		void(*cb)(void* context, plugin_param_t* param));

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


void plugin_close(void* plug) {
	if (plug) {
		FreeLibrary(plug);
		free(plug);
	}
}

#define PLUG_CLOSE( plug) plugin_close(plug);plug=NULL;

void* plugin_open(const char* path) {
	plugin_t* plug = malloc(sizeof(plugin_t));
	memset(plug, 0, sizeof(plugin_t));

#ifdef _WIN32
	plug->handle = LoadLibrary(path);
	if (plug->handle == NULL) {
		_GetLastError(plug);
//		if (err) {
//			strncpy(err->data, plug->error, err->size);
//		}
		free(plug);
		return NULL;
	}
	typedef void(*FUNCADDR)();
	plug->plugin_call = (FUNCADDR)(GetProcAddress(plug->handle, "plug_call"));
	if (!plug->plugin_call) {
//		if (err) {
//			strncpy(err->data, "no 'plug_call' symbol in plug",err->size);
//		}
		PLUG_CLOSE(plug);
		return NULL;
	}


#else
#endif
	return plug;
}

void  plugin_call(void* plugin, plugin_param_t* in, void* context, 
	              void(*cb)(void* context, plugin_param_t* param,int status)) {
	((plugin_t*)plugin)->plugin_call(plugin, in, context,cb);
}

void  plugin_set_notification(void* plugin, void* context, 
	                          void(*cb)(void* context, plugin_param_t* param)) {

	((plugin_t*)plugin)->plugin_set_notification(plugin, context, cb);
}