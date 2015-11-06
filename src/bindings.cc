#include <node.h>
#include <nan.h>
#include "ActionHandler.h"

using namespace v8;

void function_init(Handle<Object> exports) {
    exports->Set(NanNew<String>("findAlljoynServices"),
        NanNew<FunctionTemplate>(findAlljoynServices)->GetFunction());
    exports->Set(NanNew<String>("updateBusName"),
        NanNew<FunctionTemplate>(updateBusName)->GetFunction());
    exports->Set(NanNew<String>("getDeviceName"),
        NanNew<FunctionTemplate>(getDeviceName)->GetFunction());
    exports->Set(NanNew<String>("updateInterface"),
        NanNew<FunctionTemplate>(updateInterface)->GetFunction());
    exports->Set(NanNew<String>("getInterfaceName"),
        NanNew<FunctionTemplate>(getInterfaceName)->GetFunction());
    exports->Set(NanNew<String>("getActionName"),
        NanNew<FunctionTemplate>(getActionName)->GetFunction());
    exports->Set(NanNew<String>("doAction"),
        NanNew<FunctionTemplate>(doAction)->GetFunction());
}

NODE_MODULE(node_alljoyn, function_init)
