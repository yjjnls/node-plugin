

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
	
//	struct plugin_callback_param_t{
//		plugin_param_t value;
//		int            status; 
//	};
//	typedef struct plugin_callback_param_t plugin_callback_param_t;
//	typedef void(*plugin_callback_t)(plugin_callback_param_t* param );


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif
	
	void* plugin_open(const char* path);
	
	void  plugin_call(void* plugin, plugin_param_t* in, void* context, 
		  void(*cb)(void* context, plugin_param_t* param, int status));
	
	void  plugin_set_notification(void* plugin, void* context,
		  void(*cb)(void* context, plugin_param_t* param));

	void  plugin_close(void* plugin);

#ifdef __cplusplus
}
#endif

#endif