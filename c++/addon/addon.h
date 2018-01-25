#ifndef TEST_ADDONS_NAPI_6_OBJECT_WRAP_NodePlugin_H_
#define TEST_ADDONS_NAPI_6_OBJECT_WRAP_NodePlugin_H_

#include <node_api.h>
#include <uv.h>
#include <map>
#include <list>
#include <string>
#include "function-def.h"
class Addon;

struct async_callback_param_t {

	async_callback_param_t()
		:  data(NULL), size(0), status(0)
		, finalize(NULL), hint(NULL)
	{}

	const void* data;
	size_t      size;
	int         status;
	NODE_PLUGIN_FINALIZE finalize;
	void*        hint;


};

struct async_callback_t : public async_callback_param_t {
	async_callback_t(Addon* p)
		: addon(p), ref(nullptr)
	{}
	Addon*     addon;
	napi_ref   ref;

};

class Addon {
	enum state_t {
		IDLE =0,
		INIT   ,//initializing
		RUN    ,//running
		TERM    //terminating
	};
 public:
  static napi_value Init(napi_env env, napi_value exports);
  static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

  inline void PushCallback(async_callback_t* ac)
  {
	  uv_mutex_lock(&mutext_);
	  callbacks_.push_back(ac);
	  uv_mutex_unlock(&mutext_);
	  uv_async_send(&async_);
  }

  inline void emit_plugin_terminated()
  {
	  uv_mutex_lock(&mutext_);
	  plugin_terminated_ = true;
	  uv_mutex_unlock(&mutext_);
	  uv_async_send(&async_);

  }

 private:
  explicit Addon(const std::string& basename);
  ~Addon();

  static napi_value New(napi_env env, napi_callback_info info);
  static napi_value Release(napi_env env, napi_callback_info info);
  static napi_value AsyncCall(napi_env env, napi_callback_info info);
  static napi_value Initialize(napi_env env, napi_callback_info info);
  
  /////
  static void OnAsync(uv_async_t* handle);
  void Call(napi_env env, napi_value param, napi_value callback);
  bool Initialize(const std::string& dir, const std::string& options ,napi_value notify);

  void OnAsync();
  bool Open(const std::string& dir);
  const std::string error() const { return error_; }
  napi_status to_buffer(async_callback_param_t* cb, napi_value* pvalue);
  void Release(napi_value callback);
  void Terminate();

//  void RegistNotification(napi_value callback);
//  ////
//  NODE_PLUGIN_CALL         plugin_call;
//  NODE_PLUGIN_INIT         plugin_init;
//  NODE_PLUGIN_TERMINATE    plugin_terminate;
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

  //napi_value callback_;
//  napi_ref callback_ref_;


  uv_mutex_t      mutext_;
  uv_async_t      async_;
  std::list<async_callback_t*>       callbacks_;
  std::list<async_callback_param_t*> notifications_;
  std::string error_;
  std::string basename_; //filenmae of the plutin
  void*       handle_;//dynamic lib handle

  napi_value notifier_;
  napi_ref   notifier_ref_;
  napi_value terminater_;
  napi_ref   terminater_ref_;

  node_plugin_interface_t* plugin_;
  state_t state_;
  bool    plugin_terminated_;
};

#endif  // TEST_ADDONS_NAPI_6_OBJECT_WRAP_NodePlugin_H_
