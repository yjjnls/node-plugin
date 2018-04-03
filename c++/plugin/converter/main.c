#include "../../addon/plugin_interface.h"
//#include <stdio.h>
//#include <string.h>
//#include <malloc.h>
//#include <string>
#include <ctype.h>

int convert(plugin_buffer_t* data,
			plugin_buffer_t* meta,
			plugin_buffer_t* result);

#define VERSION "0.1.1"
///////////////////////////////////////////////////////////////
static void init (const plugin_interface_t* self,
	              const void*               context,
	              const plugin_buffer_t*    data,
	              plugin_callback_fn        callback)
{
	if (data) {
	}

    if (callback){
		callback(self, context, 0, NULL);
    }
}

static void _release(plugin_buffer_t* self) {
	if (self->data) {
		free( self->data);
		self->data = NULL;
	}
	free( self);
}


static void call(const plugin_interface_t* self,
	             const void*               context,
	             plugin_buffer_t*          data,
	             plugin_buffer_t*          meta,
	             plugin_callback_fn        callback)
{
	plugin_buffer_t result;
	int status = convert(data, meta, &result);
	callback(self, context, status, &result);
	return;
}

static void terminate(const plugin_interface_t* self,
	                  const void*               context,
	                  plugin_callback_fn        callback)
{
    if (callback)
    {
		callback(self,context, 0, NULL);
    }
}

PLUGIN_INTERFACE(VERSION, init, call, terminate);




static int convert(plugin_buffer_t* data,
	plugin_buffer_t* meta,
	plugin_buffer_t* result)
{
	if (!data) {
		plugin_buffer_string_set(result, "string is null.");
		return 1;
	}

	if (!meta) {
		plugin_buffer_string_set(result, "action is null.");
		return 1;
	}

	const char* action = (const char*)meta->data;
	size_t size = meta->size;

	plugin_buffer_safe_move(data, result);
	char* txt = (char*)result->data;
	if (!strncmp(action, "upper", size))
	{
		for (int i = 0; i < result->size; i++)
		{
			txt[i]=(char)toupper(txt[i]);
		}
	}
	else if (!strncmp(action, "lower", size))
	{
		for (int i = 0; i < result->size; i++)
		{
			txt[i] = (char)tolower(txt[i]);
		}
	}
	else {
		plugin_buffer_string_set(result, "action not support.");
		return 1;

	}
	return 0;
}