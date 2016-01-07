#include <node.h>
#include <nan.h>
#include "ActionHandler.h"
#include "EventHandler.h"
#include "ControlPanelHandler.h"

using namespace v8;

void function_init(Handle<Object> exports) {
//Action
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
//Event
    exports->Set(NanNew<String>("findAlljoynEventServices"),
        NanNew<FunctionTemplate>(findAlljoynEventServices)->GetFunction());
    exports->Set(NanNew<String>("updateEventBusName"),
        NanNew<FunctionTemplate>(updateEventBusName)->GetFunction());
    exports->Set(NanNew<String>("getEventDeviceName"),
        NanNew<FunctionTemplate>(getEventDeviceName)->GetFunction());
    exports->Set(NanNew<String>("updateEventInterface"),
        NanNew<FunctionTemplate>(updateEventInterface)->GetFunction());
    exports->Set(NanNew<String>("getEventInterfaceName"),
        NanNew<FunctionTemplate>(getEventInterfaceName)->GetFunction());
    exports->Set(NanNew<String>("getEventName"),
        NanNew<FunctionTemplate>(getEventName)->GetFunction());
    exports->Set(NanNew<String>("doEvent"),
        NanNew<FunctionTemplate>(doEvent)->GetFunction());        
//ControlPanel
    exports->Set(NanNew<String>("findControlPanelServices"),
        NanNew<FunctionTemplate>(findControlPanelServices)->GetFunction());
    exports->Set(NanNew<String>("getControlPanel"),
        NanNew<FunctionTemplate>(getControlPanel)->GetFunction());  
    exports->Set(NanNew<String>("getControlPanelDeviceName"),
        NanNew<FunctionTemplate>(getControlPanelDeviceName)->GetFunction());     
    exports->Set(NanNew<String>("getProperty"),
        NanNew<FunctionTemplate>(getProperty)->GetFunction());  
    exports->Set(NanNew<String>("setProperty"),
        NanNew<FunctionTemplate>(setProperty)->GetFunction());        
}

NODE_MODULE(node_alljoyn, function_init)
