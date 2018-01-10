#ifndef TEST_ADDONS_NAPI_6_OBJECT_WRAP_NodePlugin_H_
#define TEST_ADDONS_NAPI_6_OBJECT_WRAP_NodePlugin_H_

#include <node_api.h>
#include <uv.h>
#include <map>
#include <list>
#include <string>
#include "function-def.h"
class Plugin;
struct async_callback_t {
	async_callback_t(Plugin* p)
		: plugin(p), ref(nullptr)
		, data(NULL), size(0), status(0)
		, finalize(NULL), hint(NULL)
	{}
	Plugin* plugin;
	napi_ref   ref;

	const void*       data;
	size_t      size;
	int         status;
	NODE_PLUGIN_FINALIZE finalize;
	void*        hint;


};

class Plugin {
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

  inline void PushNotification(async_callback_t* ac)
  {
	  uv_mutex_lock(&mutext_);
	  notifications_.push_back(ac);
	  uv_mutex_unlock(&mutext_);
	  uv_async_send(&async_);
  }

 private:
  explicit Plugin(const std::string& path);
  ~Plugin();

  static napi_value New(napi_env env, napi_callback_info info);
//  static napi_value GetValue(napi_env env, napi_callback_info info);
//  static napi_value SetValue(napi_env env, napi_callback_info info);
//  static napi_value PlusOne(napi_env env, napi_callback_info info);
  static napi_value Release(napi_env env, napi_callback_info info);
//
//  static napi_value Call(napi_env env, napi_callback_info info);
  static napi_value AsyncCall(napi_env env, napi_callback_info info);
  /////
  static void OnAsync(uv_async_t* handle);
  void Call(napi_env env, napi_value param, napi_value callback);
  void OnAsync();
  bool Open();
  const std::string error() const { return error_; }
  napi_status to_buffer(async_callback_t* cb, napi_value* pvalue);
  void Release();
  ////
  NODE_PLUGIN_CALL      plugin_call;
  NODE_PLUGIN_SET_CALLBACK plugin_set_callback;

  static void plugin_callback(void* context,
	  const void* data, size_t size, int   status,
	  NODE_PLUGIN_FINALIZE finalize, void* hint);

  static void plugin_notify(void* context,
	  const void* data, size_t size, int   status,
	  NODE_PLUGIN_FINALIZE finalize, void* hint);










  static napi_ref constructor;
  double value_;
  napi_env env_;
  napi_ref wrapper_;

  //napi_value callback_;
  napi_ref callback_ref_;


  uv_mutex_t      mutext_;
  uv_async_t      async_;
  std::list<async_callback_t*> callbacks_;
  std::list<async_callback_t*> notifications_;
  std::string path_;
  std::string error_;
  void*           handle_;//dynamic lib handle
};

#endif  // TEST_ADDONS_NAPI_6_OBJECT_WRAP_NodePlugin_H_
