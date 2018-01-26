
#include <assert.h>
#include <stdio.h>
#include "_dlfcn.h"
#include "addon.h"

napi_ref Addon::constructor;
Addon::Addon(const std::string& basename)
	: env_(nullptr), wrapper_(nullptr)
	, handle_(NULL)
	, plugin_(nullptr)
	, basename_(basename)
	, notifier_ref_(nullptr)
	, state_(Addon::IDLE)
	, plugin_terminated_(false)
	, terminater_ref_(nullptr)
{
	uv_async_init(uv_default_loop(), &async_, Addon::OnEvent);
	async_.data = this;

	uv_mutex_init(&mutext_);
}

Addon::~Addon()
{
	napi_delete_reference(env_, wrapper_);

}


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

void Addon::plugin_call_return(const void* self, const void* context,
	const void* data, size_t size,
	int status,
	node_plugin_finalize_fn finalizer,
	void* hint)
{

	node_plugin_interface_t* iface = (node_plugin_interface_t*)self;
	Addon* This = static_cast<Addon*>(iface->addon_);


	async_callback_t* ac = static_cast<async_callback_t*>((void*)context);
	ac->data = data;
	ac->size = size;
	ac->status = status;
	ac->finalize = finalizer;
	ac->hint = hint;
	if (finalizer == NULL)
	{
		ac->data = _buffer_copy(data, size);
		ac->finalize = _buffer_copy_finalize;
	}

	uv_mutex_lock(&This->mutext_);
	This->callbacks_.push_back(ac);
	uv_mutex_unlock(&This->mutext_);
	uv_async_send(&This->async_);

}


void Addon::plugin_notify(const void* self, 
	const void* data, size_t size,
	node_plugin_finalize_fn finalizer,
	void* hint)

{
	
	const void* d   = data;
	size_t      len = size;
	node_plugin_finalize_fn f = finalizer;
	if (f==NULL)
	{
		d = _buffer_copy(data, size);
		f = _buffer_copy_finalize;
	}

	node_plugin_interface_t* iface = (node_plugin_interface_t*)self;
	Addon* This = static_cast<Addon*>(iface->addon_);
	uv_mutex_lock(&This->mutext_);
	This->notifications_.push_back(async_callback_param_t(
		d,len,f,hint
	));
	uv_mutex_unlock(&This->mutext_);
	uv_async_send(&This->async_);
}


void Addon::OnEvent(uv_async_t* handle)
{
	Addon* obj = (Addon*)handle->data;
	obj->OnEvent();
}



void Addon::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  reinterpret_cast<Addon*>(nativeObject)->~Addon();
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Addon::Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor properties[] = {
	  DECLARE_NAPI_METHOD("initialize", Initialize),
      DECLARE_NAPI_METHOD("call", Call),
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

napi_value Addon::New(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value target;
  status = napi_get_new_target(env, info, &target);
  assert(status == napi_ok);
  bool is_constructor = target != nullptr;

  if (is_constructor) {
    // Invoked as constructor: `new Addon(...)`
    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    assert(status == napi_ok);

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    assert(status == napi_ok);

	char basename[FILENAME_MAX];

    if (valuetype != napi_undefined) {
	  size_t len;
      status =  napi_get_value_string_utf8(env, args[0], basename, FILENAME_MAX, &len);
      assert(status == napi_ok);
    }

    Addon* obj = new Addon(basename);

	
    obj->env_ = env;
    status = napi_wrap(env,
                       jsthis,
                       reinterpret_cast<void*>(obj),
                       Addon::Destructor,
                       nullptr,  // finalize_hint
                       &obj->wrapper_);
    assert(status == napi_ok);

    return jsthis;
  } else {
    // Invoked as plain function `Addon(...)`, turn into construct call.
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





napi_value Addon::Initialize(napi_env env, napi_callback_info info) {
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
	status = napi_get_value_string_utf8(env, args[1], (char*)options.data(), options.size(), &n);
	assert(status == napi_ok);

	
	status = napi_typeof(env, args[2], &type);
	assert(status == napi_ok);
	
	Addon* obj;
	status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
	
	obj->Initialize(dir, options, args[2]);

	return nullptr;
}

void Addon::Call(napi_env env, napi_value param, napi_value callback)
{
	napi_status status;

	async_callback_t* ac = new async_callback_t(this);
	status = napi_create_reference(env, callback, 1, &ac->ref);
	assert(status == napi_ok);

	void* data = NULL;
	size_t length = 0;
	status = napi_get_buffer_info(env_, param, &data, &length);
	assert(status == napi_ok);

	plugin_->call(plugin_, ac, (const char*)data, length);
}

napi_value Addon::Release(napi_env env, napi_callback_info info)
{
	Addon* obj;
	size_t argc = 3;
	napi_value args[3];
	napi_value jsthis;
	napi_status status;
	status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
	assert(status == napi_ok);
	
	status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
	assert(status == napi_ok);
	obj->Release(args[0]);
	return nullptr;
}

napi_value Addon::Call(napi_env env, napi_callback_info info) 
{
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

	Addon* obj;
	status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
	assert(status == napi_ok);
	obj->Call(env, args[0], cb);
	
	return nullptr;
}

napi_status Addon::to_buffer(async_callback_param_t* ac,napi_value* pvalue)
{
	napi_status status;
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

void Addon::OnEvent() 
{
	napi_status status;

	std::list<async_callback_t*> cbs;
	std::list<async_callback_param_t> ntfs;
	bool terminated = false;
	uv_mutex_lock(&mutext_);
	terminated = plugin_terminated_;

	if (!terminated)
	{
		cbs.swap(callbacks_);
		ntfs.swap(notifications_);
	}
	uv_mutex_unlock(&mutext_);
	if (terminated) 
	{
		this->Terminate();
		return;
	}

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

		if (ac->finalize)
		{
			ac->finalize(env_, (void*)ac->data, ac->hint);
		}
		delete ac;
	}

	for (std::list<async_callback_param_t>::iterator it = ntfs.begin();
		it != ntfs.end(); it++) {

		async_callback_param_t& ac = *it;
		napi_value cb;
		
		status = napi_get_reference_value(env_, this->notifier_ref_, &cb);
		assert(status == napi_ok);
		napi_value result;
		napi_value argv[1];

		status = to_buffer(&ac, &argv[0]);
		assert(status == napi_ok);


		status = napi_call_function(env_, global, cb, 1, argv, &result);
		assert(status == napi_ok);
		if (ac.finalize)
		{
			ac.finalize(env_, (void*)ac.data, ac.hint);
		}

	}
	napi_close_handle_scope(env_, scope);
}


bool Addon::Open(const std::string& dir)
{
	char _CWD[FILENAME_MAX];
	_getcwd(_CWD, sizeof(_CWD));
	_chdir(dir.c_str());
	handle_ = _dlopen(basename_.c_str());
	_chdir(_CWD);

	if (!handle_)
	{
		//error_ = _dlerror();
		error_ = error_ + "("+basename_ + "@" + dir+")";
		return false;
	}
	node_plugin_interface_initialize =
		(node_plugin_interface_initialize_fn)_dlsym(handle_, "node_plugin_interface_initialize");
	if (!node_plugin_interface_initialize)
	{
		error_ = "invalid plugin, no <node_plugin_interface_initialize>";
		_dlclose(handle_);
		handle_ = NULL;
		return false;
	}

	node_plugin_interface_terminate =
		(node_plugin_interface_terminate_fn)_dlsym(handle_, "node_plugin_interface_terminate");
	
	if (!node_plugin_interface_terminate)
	{
		error_ = "invalid plugin, no <node_plugin_interface_terminate>";
		_dlclose(handle_);
		handle_ = NULL;
		return false;
	}
	return true;
}

bool Addon::Initialize(const std::string& dir, const std::string& options, napi_value notify)
{
	state_ = Addon::INIT;
	if (!Open(dir))
	{
		napi_throw_error(env_, "127", error().c_str());
		return false;
	}

	napi_valuetype type;
	napi_status status;

	node_plugin_notify_fn ncb = NULL;
	status = napi_typeof(env_, notify, &type);
	assert(status == napi_ok);
	if (type != napi_undefined)
	{
		ncb    = Addon::plugin_notify;
		status = napi_create_reference(env_, notify, 1, &notifier_ref_);
	}

	plugin_ = node_plugin_interface_initialize(this, 
		Addon::plugin_call_return, Addon::plugin_notify);
	plugin_->addon_ = this;
	plugin_->init(plugin_, options.data(), options.size());
	state_ = Addon::INIT;
	return true;

}
void Addon::terminate_done(const void* self)
{
	node_plugin_interface_t* iface = (node_plugin_interface_t*)self;
	Addon* This = (Addon*)iface->addon_;

	uv_mutex_lock(&This->mutext_);
	This->plugin_terminated_ = true;
	uv_mutex_unlock(&This->mutext_);
	uv_async_send(&This->async_);
}

void Addon::Terminate()
{
	
	if (terminater_ref_)
	{
		napi_status status;
		napi_handle_scope scope;

		status = napi_open_handle_scope(env_, &scope);

		napi_value global;
		status = napi_get_global(env_, &global);
		assert(status == napi_ok);

		napi_value result;
		napi_value argv[1];
		napi_value  done=nullptr;

		status = napi_get_reference_value(env_, terminater_ref_, &done);

		status = napi_call_function(env_, global, done, 0, argv, &result);
		assert(status == napi_ok);
		napi_close_handle_scope(env_,scope);

		uint32_t refcount;
		napi_reference_unref(env_, terminater_ref_, &refcount);
		napi_delete_reference(env_, terminater_ref_);
		terminater_ref_ = nullptr;

	}
	if (plugin_)
	{
		node_plugin_interface_terminate(plugin_);
		plugin_ = nullptr;
	}


	if (handle_) {
		_dlclose(handle_);
		handle_ = NULL;
	}
	uv_mutex_destroy(&mutext_);
	uv_close((uv_handle_t*)&async_, NULL);

}
void Addon::Release(napi_value callback)
{
	napi_valuetype type;
	napi_status status;
	status = napi_typeof(env_, callback, &type);
	assert(status == napi_ok);
	if (type == napi_function)
	{
		status = napi_create_reference(env_, callback, 1, &terminater_ref_);
	}

	plugin_->terminate(plugin_, Addon::terminate_done);

}



















napi_value Init(napi_env env, napi_value exports) {
  return Addon::Init(env, exports);
}

NAPI_MODULE(node_plugin, Init)
