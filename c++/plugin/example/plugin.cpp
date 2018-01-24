#ifdef _WIN32
	#define _DLLEXPORT __declspec(dllexport)
#else
	#define _DLLEXPORT  
#endif
#include "../../addon/function-def.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string>
#include "calculator.hpp"
static	NODE_PLUGIN_CALLBACK g_callback = NULL;
static	NODE_PLUGIN_CALLBACK g_notify = NULL;
static  const void*          g_context = NULL;
static  int counter_ = 0;

//@NODE_PLUGIN_CALL
extern "C" _DLLEXPORT 
void plugin_call(const void* data, size_t size ,void* context )
{
	
	static int n=0;
	n++;
	
	printf("@%x @%x\n", context, g_callback);
	if( context && g_callback )
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
		printf("\n%s =>%s\n", data, retval);
		printf("g_callback begin %x\n", g_callback);
		g_callback(context, retval, strlen(retval)+1,0,NULL,NULL);
		printf("g_callback end %x\n", g_callback);
	}

	counter_++;
	if( g_notify )
	{
		char log[256];
		sprintf(log,"* %d request has been procced.", counter_);
		g_notify(g_context,log,strlen(log)+1,0,NULL,NULL);
	}
	
//	free(buf);
	
}

//@NODE_PLUGIN_SET_CALLBACK
extern "C" _DLLEXPORT 		
void plugin_set_callback( 
	NODE_PLUGIN_CALLBACK cb,
	NODE_PLUGIN_CALLBACK notify)
{
	g_callback = cb;
	g_notify   = g_notify;
}

extern "C" _DLLEXPORT
int plugin_init(const void* data, size_t size /*JSON */,
	const void* context,
	NODE_PLUGIN_CALLBACK cb,
	NODE_PLUGIN_CALLBACK notify)
{
	g_callback = cb;
	g_notify = notify;
	g_context = context;
	return 0;
}

extern "C" _DLLEXPORT
void plugin_terminate( void(*done)(void) )
{
	printf("plugin_release!\n");
	//done();
}

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

static void terminate(const void* self, void(*done)())
{
	if (done) {
		done();
	}
}

extern "C" _DLLEXPORT
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