
#include <iostream>
#include "plugin.h"
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

	Plugin::Plugin(const std::string& path)
		: path_(path)
	{
	}

	Plugin::~Plugin() {
	}

	void Plugin::Init(Local<Object> exports) {
		Isolate* isolate = exports->GetIsolate();

		// Prepare constructor template
		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
		tpl->SetClassName(String::NewFromUtf8(isolate, "Plugin"));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		// Prototype
		NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);

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



			Plugin* obj = new Plugin(std::string(*path));
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
		Plugin* obj = ObjectWrap::Unwrap<Plugin>(args.Holder());
		
		

//		std::cout << "<" << obj->name_ << ">" << std::endl;

		args.GetReturnValue().Set(Number::New(isolate, n++));
	}



} 