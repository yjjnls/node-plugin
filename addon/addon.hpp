#ifndef __NODE_PLUGIN_ADDON_H
#define __NODE_PLUGIN_ADDON_H

#include <node.h>
#include <node_object_wrap.h>
#include <string>
#include <vector>


namespace node_plugin {


	class Plugin : public node::ObjectWrap {
	public:
		static void Init(v8::Local<v8::Object> exports);

	private:
		explicit Plugin(const std::string& path_);
		~Plugin();

		
		static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
		static v8::Persistent<v8::Function> constructor;


		static void Open(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Call(const v8::FunctionCallbackInfo<v8::Value>& args);
		

		std::string path_;//path of the module
		
	};

}  

#endif