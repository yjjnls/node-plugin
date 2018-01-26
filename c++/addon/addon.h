#ifndef _NODE_PLUGIN_ADDON_H_
#define _NODE_PLUGIN_ADDON_H_

#include <node_api.h>
#include <uv.h>
#include <map>
#include <list>
#include <string>
#include "function-def.h"
class Addon;

struct async_callback_param_t {

	async_callback_param_t()
		: data(NULL), size(0)
		, finalize(NULL), hint(NULL)
	{}

	async_callback_param_t(const void* d,size_t len,
		NODE_PLUGIN_FINALIZE finalizer, void* hinter)
		: data(d), size(len)
		, finalize(finalizer), hint(hinter)
	{}

	const void* data;
	size_t      size;
	NODE_PLUGIN_FINALIZE finalize;
	void*        hint;


};

struct async_callback_t : public async_callback_param_t {
	async_callback_t(Addon* p)
		: addon(p), ref(nullptr),status(0)
	{}

	Addon*     addon;
	napi_ref   ref;
	int         status;

};

class Addon {
	enum state_t {
		IDLE = 0,
		INIT,//initializing
		RUN,//running
		TERM    //terminating
	};
public:
	static napi_value Init(napi_env env, napi_value exports);
	static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);



private:
	explicit Addon(const std::string& basename);
	~Addon();

	static napi_value New(napi_env env, napi_callback_info info);
	static napi_value Initialize(napi_env env, napi_callback_info info);
	static napi_value Call(napi_env env, napi_callback_info info);
	static napi_value Release(napi_env env, napi_callback_info info);
	static void       OnEvent(uv_async_t* handle);

	/////
	void Call(napi_env env, napi_value param, napi_value callback);
	bool Initialize(const std::string& dir, const std::string& options, napi_value notify);

	void OnEvent();
	bool Open(const std::string& dir);
	const std::string error() const { return error_; }
	napi_status to_buffer(async_callback_param_t* cb, napi_value* pvalue);
	void Release(napi_value callback);
	void Terminate();


	node_plugin_interface_initialize_fn node_plugin_interface_initialize;
	node_plugin_interface_terminate_fn  node_plugin_interface_terminate;

	static void terminate_done(const void* self);
	static void plugin_call_return(const void* self, const void* context,
		const void* data, size_t size,
		int status,
		node_plugin_finalize_fn finalizer,
		void* hint);

	static void plugin_notify(const void* self,
		const void* data, size_t size,
		node_plugin_finalize_fn finalizer,
		void* hint);





	static napi_ref constructor;
	napi_env env_;
	napi_ref wrapper_;

	uv_mutex_t      mutext_;
	uv_async_t      async_;
	std::list<async_callback_t*>       callbacks_;
	std::list<async_callback_param_t> notifications_;
	std::string error_;
	std::string basename_; //filenmae of the plutin
	void*       handle_;//dynamic lib handle

	napi_ref   notifier_ref_;
	napi_ref   terminater_ref_;

	node_plugin_interface_t* plugin_;
	state_t state_;
	bool    plugin_terminated_;
};

#endif