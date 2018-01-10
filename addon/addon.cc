#include "plugin.h"

napi_value Init(napi_env env, napi_value exports) {
  return Plugin::Init(env, exports);
}

NAPI_MODULE(node_plugin, Init)
