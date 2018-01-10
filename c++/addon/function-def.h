
#ifndef _NODE_PLUGIN_FUNCTION_PROTOTYPE_DEF_H_
#define _NODE_PLUGIN_FUNCTION_PROTOTYPE_DEF_H_


#ifdef __cplusplus
extern "C"{
#endif

/**
 * function called by Javascript(addon) which implement in plugin (c/c++)
*/
/*node_plugin_call*/
typedef void (* NODE_PLUGIN_CALL)(
                const void* data, size_t size, /*Javascript passed node::Buffer*/ 
                void* context /* if context == NULL,there will be no callback called*/
		);
				 
typedef void(*NODE_PLUGIN_FINALIZE)(
	         const void* env,
	         void* finalize_data,
		     void* finalize_hint);


typedef void (* NODE_PLUGIN_CALLBACK )(
				void* context, /*context which set in plugin, or NULL for notification*/
				const void* data, size_t size, /*to pass to Javascript as node::Buffer*/
				int   status, /*optional ,normally 0 mean success */
	            NODE_PLUGIN_FINALIZE finalize,/*optional callback to call the data collected*/
				void* hint /*optional hint to pass to the finalize callback during collection. */
		);
/*node_plugin_set_callback*/		
typedef void (* NODE_PLUGIN_SET_CALLBACK )( 
				NODE_PLUGIN_CALLBACK cb , /*optional callback for plugin_call. */
				NODE_PLUGIN_CALLBACK notify /*optional notify (no call).*/
		  );
		
#ifdef __cplusplus
}
#endif



#endif