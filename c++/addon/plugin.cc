
#include <assert.h>
#include <stdio.h>
#include "_dlfcn.h"
#include "plugin.h"

napi_ref Plugin::constructor;

static void _buffer_copy_finalize(
	const void* env,
	void* finalize_data,
	void* finalize_hint)
{
	free(finalize_data);
}

static void* _buffer_copy(const void* indata, int size)
{
	void* buf = malloc(size);
	memcpy(buf, indata, size);
	return buf;
}

static void _plugin_finalize(NODE_PLUGIN_FINALIZE, void* finalize_data,void* finalize_hint)
{


}

void Plugin::plugin_callback(const void* context,
	const void* data, size_t size, int   status,
	NODE_PLUGIN_FINALIZE finalize, void* hint)
{
	async_callback_t* ac = static_cast<async_callback_t*>((void*)context);
	ac->data = data;
	ac->size = size;
	ac->status = status;
	ac->finalize = finalize;
	ac->hint = hint;
	if (finalize == NULL)
	{
		ac->data = _buffer_copy(data, size);
		ac->finalize = _buffer_copy_finalize;
	}
	
	ac->plugin->PushCallback(ac);
	

}

void Plugin::plugin_notify(const void* context,
	const void* data, size_t size, int   status,
	NODE_PLUGIN_FINALIZE finalize, void* hint)
{
	Plugin* This = static_cast<Plugin*>((void*)context);
	async_callback_param_t* p = new async_callback_param_t;

	p->data = data;
	p->size = size;
	p->status = status;
	p->finalize = finalize;
	p->hint = hint;
	if (finalize == NULL)
	{
		p->data = _buffer_copy(data, size);
		p->finalize = _buffer_copy_finalize;
	}
	uv_mutex_lock(&This->mutext_);
	This->notifications_.push_back(p);
	uv_mutex_unlock(&This->mutext_);
	uv_async_send(&This->async_);
}















void Plugin::OnAsync(uv_async_t* handle)
{
	Plugin* obj = (Plugin*)handle->data;
	obj->OnAsync();
}

Plugin::Plugin(const std::string& basename)
    : env_(nullptr), wrapper_(nullptr)
	, handle_(NULL)
	, basename_(basename)
	, plugin_call(NULL), notifier_(NULL)
{
	uv_async_init(uv_default_loop(),&async_, Plugin::OnAsync);
	async_.data = this;
	
	uv_mutex_init(&mutext_);
}

Plugin::~Plugin() 
{ 
	napi_delete_reference(env_, wrapper_);

}

void Plugin::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  reinterpret_cast<Plugin*>(nativeObject)->~Plugin();
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Plugin::Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor properties[] = {
	  DECLARE_NAPI_METHOD("initialize", Initialize),
      DECLARE_NAPI_METHOD("call", AsyncCall),
      DECLARE_NAPI_METHOD("release", Release)
  };
  
  napi_value cons;
  status =
      napi_define_class(env, "Plugin", NAPI_AUTO_LENGTH, New, nullptr, 3, properties, &cons);
  assert(status == napi_ok);

  status = napi_create_reference(env, cons, 1, &constructor);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "Plugin", cons);
  assert(status == napi_ok);
  return exports;
}

napi_value Plugin::New(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value target;
  status = napi_get_new_target(env, info, &target);
  assert(status == napi_ok);
  bool is_constructor = target != nullptr;

  if (is_constructor) {
    // Invoked as constructor: `new Plugin(...)`
    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    assert(status == napi_ok);

    //double value = 0;
	

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    assert(status == napi_ok);

	//char path[FILENAME_MAX];
	char basename[FILENAME_MAX];

    if (valuetype != napi_undefined) {
	  size_t len;
      status =  napi_get_value_string_utf8(env, args[0], basename, FILENAME_MAX, &len);
      assert(status == napi_ok);
    }

//	status = napi_typeof(env, args[1], &valuetype);
//	assert(status == napi_ok);
//	if (valuetype != napi_undefined) {
//		size_t len;
//		status = napi_get_value_string_utf8(env, args[1], basename, FILENAME_MAX, &len);
//		assert(status == napi_ok);
//	}

    Plugin* obj = new Plugin(basename);

	
    obj->env_ = env;
    status = napi_wrap(env,
                       jsthis,
                       reinterpret_cast<void*>(obj),
                       Plugin::Destructor,
                       nullptr,  // finalize_hint
                       &obj->wrapper_);
    assert(status == napi_ok);

    return jsthis;
  } else {
    // Invoked as plain function `Plugin(...)`, turn into construct call.
    size_t argc_ = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr);
    assert(status == napi_ok);

    const size_t argc = 1;
    napi_value argv[argc] = {args[0]};

    napi_value cons;
    status = napi_get_reference_value(env, constructor, &cons);
    assert(status == napi_ok);

    napi_value instance;
    status = napi_new_instance(env, cons, argc, argv, &instance);
    assert(status == napi_ok);

    return instance;
  }
}



void Plugin::Call(napi_env env, napi_value param, napi_value callback)
{
	napi_status status;

	async_callback_t* ac = new async_callback_t(this);
	status = napi_create_reference(env, callback, 1, &ac->ref);
	assert(status == napi_ok);

	void* data = NULL;
	size_t length=0;
	status = napi_get_buffer_info(env_, param, &data, &length);
	assert(status == napi_ok);
	this->plugin_call((const char*)data, length, ac);
}

napi_value Plugin::Release(napi_env env, napi_callback_info info) 
{
	Plugin* obj;
	size_t argc = 3;
	napi_value args[3];
	napi_value jsthis;
	napi_status status;
	status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
	assert(status == napi_ok);
	status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
	assert(status == napi_ok);
	obj->Release();
	return nullptr;
}

napi_value Plugin::Initialize(napi_env env, napi_callback_info info) {
	napi_status status;
	
	size_t argc = 3;
	napi_value args[3];
	napi_value jsthis;
	status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
	assert(status == napi_ok);
	
	napi_valuetype type;
	status = napi_typeof(env, args[0], &type);
	assert(status == napi_ok);
	assert(type == napi_string);
	std::string dir(FILENAME_MAX,0);
	size_t n;
	status = napi_get_value_string_utf8(env, args[0], (char*)dir.data(), dir.size(), &n);
	assert(status == napi_ok);

	
	status = napi_typeof(env, args[1], &type);
	assert(status == napi_ok);
	assert(type == napi_string);

	std::string options(4096, 0);
	status = napi_get_value_string_utf8(env, args[0], (char*)options.data(), options.size(), &n);
	assert(status == napi_ok);

	
	status = napi_typeof(env, args[2], &type);
	assert(status == napi_ok);
	
	Plugin* obj;
	status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
	
	obj->Initialize(dir, options, args[2]);

	return nullptr;
}
napi_value Plugin::AsyncCall(napi_env env, napi_callback_info info) {
	napi_status status;
	
	size_t argc = 3;
	napi_value args[3];
	napi_value jsthis;
	status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
	assert(status == napi_ok);

	napi_valuetype valuetype;
	status = napi_typeof(env, args[0], &valuetype);
	assert(status == napi_ok);

	void* data = NULL;
	size_t len = 0;
	//	napi_typedarray_type type;
	if (valuetype != napi_undefined) {
		status = napi_get_buffer_info(env, args[0],	&data, &len);

		assert(status == napi_ok);
	}

	napi_value cb = args[1];

	Plugin* obj;
	status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
	assert(status == napi_ok);
	obj->Call(env, args[0], cb);
	
	return nullptr;
}

napi_status Plugin::to_buffer(async_callback_param_t* ac,napi_value* pvalue)
{
	napi_status status;
	printf("=%d[%s]=", ac->size, ac->data);
	if (ac->finalize) {
		status = napi_create_external_buffer(env_,
			ac->size, (void*)ac->data,
			(napi_finalize)ac->finalize, ac->hint, pvalue);
	}
	else {
		status = napi_create_buffer_copy(
			env_, ac->size, (const void*)ac->data, NULL, pvalue);

	}
	return status;

}
void Plugin::OnAsync() 
{
	napi_status status;

	std::list<async_callback_t*> cbs;
	std::list<async_callback_param_t*> ntfs;

	uv_mutex_lock(&mutext_);
	cbs.swap(callbacks_);
	ntfs.swap(notifications_);
	uv_mutex_unlock(&mutext_);

	napi_handle_scope scope;
	status = napi_open_handle_scope(env_, &scope);
	


	napi_value global;
	status = napi_get_global(env_, &global);
	assert(status == napi_ok);

	for (std::list<async_callback_t*>::iterator it = cbs.begin();
		it != cbs.end(); it++)
	{
		async_callback_t* ac = *it;
		napi_value cb;
		status = napi_get_reference_value(env_, ac->ref, &cb);
		assert(status == napi_ok);
		napi_value result;
		napi_value argv[1];
		status = to_buffer(ac,&argv[0]);
		assert(status == napi_ok);


		status = napi_call_function(env_, global, cb, 1, argv, &result);
		assert(status == napi_ok);
	}

	for (std::list<async_callback_param_t*>::iterator it = ntfs.begin();
		it != ntfs.end(); it++) {

		async_callback_param_t* ac = *it;
		napi_value cb;
		
		status = napi_get_reference_value(env_, this->notifier_ref_, &cb);
		assert(status == napi_ok);
		napi_value result;
		napi_value argv[1];
		printf("*NOTIFI* %d %s\n", ac->size, ac->data);

		status = to_buffer(ac, &argv[0]);
		assert(status == napi_ok);


		status = napi_call_function(env_, global, cb, 1, argv, &result);
		assert(status == napi_ok);

	}
	napi_close_handle_scope(env_, scope);
}


bool Plugin::Open(const std::string& dir)
{
	char _CWD[FILENAME_MAX];
	_getcwd(_CWD, sizeof(_CWD));
	_chdir(dir.c_str());
	handle_ = _dlopen(basename_.c_str());
	_chdir(_CWD);

	if (!handle_)
	{
		error_ = _dlerror();
		return false;
	}

	return true;
	plugin_call = (NODE_PLUGIN_CALL)_dlsym(handle_, "plugin_call");
	if (!plugin_call)
	{
		error_ = "invalid plugin, no <plugin_call>";
		_dlclose(handle_);
		handle_ = NULL;
		return false;
	}


	plugin_init = (NODE_PLUGIN_INIT)_dlsym(handle_, "plugin_init");
	if (!plugin_init)
	{
		error_ = "invalid plugin, no <plugin_init>";
		_dlclose(handle_);
		handle_ = NULL;
		return false;
	}
	plugin_terminate = (NODE_PLUGIN_TERMINATE)_dlsym(handle_, "plugin_terminate");
	if (!plugin_terminate)
	{
		error_ = "invalid plugin, no <plugin_terminate>";
		_dlclose(handle_);
		handle_ = NULL;
		return false;
	}
	return true;
}

bool Plugin::Initialize(const std::string& dir, const std::string& options, napi_value notify)
{
	if (!Open(dir))
	{
		napi_throw_error(env_, "127", error().c_str());
		return false;
	}

	//plugin_set_callback(Plugin::plugin_callback);

	napi_valuetype type;
	napi_status status;

	NODE_PLUGIN_CALLBACK ncb = NULL;
	status = napi_typeof(env_, notify, &type);
	assert(status == napi_ok);
	if (type != napi_undefined)
	{
		ncb    = Plugin::plugin_notify;
		status = napi_create_reference(env_, notify, 1, &notifier_ref_);
	}
	printf("Plugin::Initialize Plugin::plugin_callback=%x ncb=%x\n", Plugin::plugin_callback, ncb);
	int code = plugin_init(options.data(), options.size(),
		this,
		Plugin::plugin_callback,
		ncb);

	if (code != 0) {
		uint32_t result;
		napi_reference_unref(env_, notifier_ref_,&result);
		napi_delete_reference(env_, notifier_ref_);

		char strcode[64];
		sprintf(strcode,"%d", code);
		napi_throw_error(env_, strcode, "plugin initialize function failed");
		return false;
	}

	return true;
}

void Plugin::Release()
{
	if (handle_) {
		_dlclose(handle_);
		handle_ = NULL;
	}
	uv_mutex_destroy(&mutext_);
	uv_close((uv_handle_t*)&async_, NULL);
}
