#include "../../addon/function-def.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string>
#include "calculator.hpp"

///////////////////////////////////////////////////////////////
static void init(const void* self, const void* data, size_t size)
{
	//TODO:
	//data is JSON string (utf8)
	//do as your needs
}

static void call(const void* self, const void* context,
	             const void* data, size_t size)
{
	static int counter = 0;
	node_plugin_interface_t* iface = (node_plugin_interface_t*)self;
	if (iface->call_return)
	{
		int status = 0;
		char retval[256];

		try
		{
			std::string expr((const char*)data, size);
			int result = calculator::eval(expr);
			sprintf(retval, "%d", result);
		}
		catch (calculator::error& e)
		{
			strcpy(retval, e.what());
			status = 1;
		}

		iface->call_return( self, context,retval, strlen(retval) + 1, status, NULL, NULL);
	}

	counter++;
	if (iface->notify)
	{
		char log[256];
		sprintf(log, "* %d request has been procced.", counter_);
		iface->notify(self, log, strlen(log) + 1, NULL, NULL);
	}
}

static void terminate(const void* self, void(*done)(const void* self))
{
	printf("plugin_terminate %x %x\n", self, done);
	if (done) {
		done(self);
	}
}

//@intialize
NODE_PLUGIN_SYMBOL 
node_plugin_interface_t* node_plugin_interface_initialize(
	void* addon,
	node_plugin_call_return_fn call_return,
	node_plugin_notify_fn      notify)
{
	node_plugin_interface_t* iface = (node_plugin_interface_t*)malloc(sizeof(node_plugin_interface_t));
	memset(iface, 0, sizeof(node_plugin_interface_t));
	iface->addon_ = addon;
	iface->call_return = call_return;
	iface->notify = notify;

	// set plugin functions
	iface->init = init;
	iface->call = call;
	iface->terminate = terminate;
	return iface;
}

//terminate
NODE_PLUGIN_SYMBOL
void node_plugin_interface_terminate(node_plugin_interface_t* iface)
{
	free(iface);
}
