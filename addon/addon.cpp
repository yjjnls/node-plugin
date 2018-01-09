
#include <iostream>
#include "plugin.h"
#include "addon.hpp"
#include <nan.h>
extern "C" {

	static void function_callback(void* context, plugin_param_t* param, int status);
	static void notify_callback(void* context, plugin_param_t* param);
	static void _async_cb(uv_async_t* handle);
	static void close_cb(uv_handle_t* handle)
	{
		printf("close the async handle!\n");
	}
}

namespace node_plugin {

	using v8::Context;
	using v8::Function;
	using v8::FunctionCallbackInfo;
	using v8::FunctionTemplate;
	using v8::Isolate;
	using v8::Local;
	using v8::Number;
	using v8::Object;
	using v8::Persistent;
	using v8::String;
	using v8::Value;
	using v8::Exception;

	Persistent<Function> Plugin::constructor;

	Plugin::Plugin()
		: handle_(NULL)
	{
		uv_mutex_init(&lock_);
		uv_async_init(uv_default_loop(), &async_, _async_cb);
		async_.data = this;
	}

	Plugin::~Plugin() {
		uv_mutex_destroy(&lock_);
		uv_close((uv_handle_t*)&async_, close_cb);
	}

	void Plugin::Init(Local<Object> exports) {
		Isolate* isolate = exports->GetIsolate();

		// Prepare constructor template
		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
		tpl->SetClassName(String::NewFromUtf8(isolate, "Plugin"));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		// Prototype
//		NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);

		NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);

		constructor.Reset(isolate, tpl->GetFunction());
		exports->Set(String::NewFromUtf8(isolate, "Plugin"),
			tpl->GetFunction());
	}

	void Plugin::New(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();

		if (args.IsConstructCall()) {

			// Invoked as constructor: `new Plugin(...)`
			if (!args[0]->IsString()) {
				isolate->ThrowException(Exception::TypeError(
					String::NewFromUtf8(isolate, "Mis type of the first param")
				));
			}
			v8::String::Utf8Value path(args[0]->ToString());



			Plugin* obj = new Plugin();
			obj->Wrap(args.This());
			args.GetReturnValue().Set(args.This());
		}
		else {
			// Invoked as plain function `Plugin(...)`, turn into construct call.
			const int argc = 1;
			Local<Value> argv[argc] = { args[0] };
			Local<Context> context = isolate->GetCurrentContext();
			Local<Function> cons = Local<Function>::New(isolate, constructor);
			Local<Object> result =
				cons->NewInstance(context, argc, argv).ToLocalChecked();
			args.GetReturnValue().Set(result);
		}
	}


	void Plugin::Open(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		static int n = 1;
		
		// Check the number of arguments passed.
		if (args.Length() < 1) {
			// Throw an Error that is passed back to JavaScript
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}

		if (!args[0]->IsString()) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Mis type of the first param")
			));
		}

		Plugin* obj = ObjectWrap::Unwrap<Plugin>(args.Holder());
		String::Utf8Value str(args[0]);
		obj->handle_ = plugin_open(*str);
		if (!obj->handle_) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Load plugin faled.")
			));
		}

//		std::cout << "<" << obj->name_ << ">" << std::endl;

//		args.GetReturnValue().Set(Number::New(isolate, n++));
	}


	void Plugin::Call(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		static int n = 1;
		

		// Check the number of arguments passed.
		if (args.Length() < 1) {
			// Throw an Error that is passed back to JavaScript
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}
		
		if (!node::Buffer::HasInstance(args[0])) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Bad argument!")));
		}


		
		Plugin* obj = ObjectWrap::Unwrap<Plugin>(args.Holder());
		callback_t* cb =NULL;
		if (args.Length() >= 2 ) {
			if (args[1]->IsFunction()) {
//				 Persistent<Function>::New(isolate, Local<Function>::Cast(args[1]));
				cb = new callback_t(obj);
				cb->cb.Reset(isolate,Local<Function>::Cast(args[1]));
			}
		}

		plugin_param_t in;
		in.data    = (char*)node::Buffer::Data(args[0]->ToObject());
		in.size    = node::Buffer::Length(args[0]->ToObject());

		
		if (!cb) {
			plugin_call(obj->handle_, &in, NULL,NULL);
		}
		else {
			plugin_call(obj->handle_, &in, cb,function_callback);
		}

//		if (!args[0]->IsString()) {
//			isolate->ThrowException(Exception::TypeError(
//				String::NewFromUtf8(isolate, "Mis type of the first param")
//			));
//		}
//
//		String::Utf8Value str(args[0]);
//		obj->handle_ = plugin_open(*str);
//		if (!obj->handle_) {
//			isolate->ThrowException(Exception::TypeError(
//				String::NewFromUtf8(isolate, "Load plugin faled.")
//			));
//		}

		//		std::cout << "<" << obj->name_ << ">" << std::endl;

		//		args.GetReturnValue().Set(Number::New(isolate, n++));
	}


	void Plugin::onsignal() {
		std::list<function_return_t> rc;
		uv_mutex_lock(&lock_);
		rc.swap(return_chain_);
		uv_mutex_unlock(&lock_);
		for (std::list<function_return_t>::iterator it = rc.begin();
			it != rc.end(); it++) {
			if (it->status == 0) {

			}
		}
		std::list<plugin_param_t> nc;
		uv_mutex_lock(&lock_);
		nc.swap(notify_chain_);
		uv_mutex_unlock(&lock_);

	}
} 

static void function_callback(void* context, plugin_param_t* param, int status)
{
	node_plugin::callback_t* cb = static_cast<node_plugin::callback_t*>(context);
	node_plugin::Plugin* plug = const_cast<node_plugin::Plugin*>(cb->plugin);
	plug->push_returns(param, status);
	
	
}

static void notify_callback(void* context, plugin_param_t* param)
{
	node_plugin::callback_t* cb = static_cast<node_plugin::callback_t*>(context);
	node_plugin::Plugin* plug = const_cast<node_plugin::Plugin*>(cb->plugin);
	plug->push_notify(param);


}

static void _async_cb(uv_async_t* handle)
{
	node_plugin::Plugin* plugin = (node_plugin::Plugin*)handle->data;
	plugin->onsignal();
	//		printf("async_cb called!\n");
	//		uv_thread_t id = uv_thread_self();
	//		printf("thread id:%lu.\n", id);
	//		uv_close((uv_handle_t*)&async, close_cb);   //如果async没有关闭，消息队列是会阻塞的  
}