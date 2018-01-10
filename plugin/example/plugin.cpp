#ifdef _WIN32
	#define _DLLEXPORT __declspec(dllexport)
#else
	#define _DLLEXPORT  
#endif
#include "../../addon/function-def.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
static	NODE_PLUGIN_CALLBACK g_callback = NULL;
static	NODE_PLUGIN_CALLBACK g_notify = NULL;

//@NODE_PLUGIN_CALL
extern "C" _DLLEXPORT 
void plugin_call(const void* data, size_t size ,void* context )
{
	
	static int n=0;
	n++;
	
	char* buf= (char*)malloc(size + 64);
	memset(buf,0,size + 64);
	sprintf(buf,"=>%d %s",n,data);
	size_t len = strlen(buf);
	
	if( context && g_callback )
	{
		g_callback(context,buf,len+1,0,NULL,NULL);
	}

	if( context && g_notify )
	{
		buf[0]='-';
		g_notify(context,buf,len+1,0,NULL,NULL);
	}
	
	free(buf);
	
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
