

#ifndef _PLUGIN_H_
#define _PLUGIN_H_


#ifdef __cplusplus
extern "C" {
#endif
	/*success errcode ==0 */
	/**
	 *
	*/
	struct plugin_buffer_t{
		const char* data;
		int         size;		
	};
	typedef struct plugin_buffer_t plugin_param_t;
	
	struct plugin_callback_param_t{
		plugin_param_t value;
		void*          context;
	};
	typedef plugin_callback_param_t plugin_callback_param_t;
	typedef void(*plugin_callback_t)(plugin_callback_param_t* param );


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif
	
	void* plugin_open(const char* path, plugin_param_t* err);
	void  plugin_call(void* plugin, plugin_param_t* in, plugin_callback_t cb);
	void  plugin_close(void* plugin);

#ifdef __cplusplus
}
#endif

#endif