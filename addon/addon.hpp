#ifndef __NODE_PLUGIN_ADDON_H
#define __NODE_PLUGIN_ADDON_H

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "plugin.h"

#include <uv.h>
namespace node_plugin {
	class Plugin;
	struct callback_t {
		callback_t(const Plugin* p)
			: plugin(p)
		{}
		const Plugin* plugin;
		v8::Persistent<v8::Function> cb;
	};


	struct function_return_t {
		plugin_param_t value;
		int            status;
	};

	class Plugin : public node::ObjectWrap {
	public:
		static void Init(v8::Local<v8::Object> exports);

		void push_returns(plugin_param_t* param, int status) {
			function_return_t fr;
			fr.status = status;
			fr.value = *param;

			uv_mutex_lock(&lock_);
			return_chain_.push_back(fr);
			uv_mutex_unlock(&lock_);
		}

		void push_notify(plugin_param_t* param) {

			uv_mutex_lock(&lock_);
			notify_chain_.push_back(*param);
			uv_mutex_unlock(&lock_);
		}
		void onsignal();
	private:
		explicit Plugin();
		~Plugin();

		
		static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
		static v8::Persistent<v8::Function> constructor;


		static void Open(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Call(const v8::FunctionCallbackInfo<v8::Value>& args);
		

		//std::string path_;//path of the module
		void* handle_;
		v8::Persistent<v8::Function> callback_;
		std::list<callback_t> functions_;
		std::list<function_return_t> return_chain_;
		std::list<plugin_param_t> notify_chain_;

		uv_mutex_t lock_;
		uv_async_t async_;



	};

}  

#endif