#include <node.h>
#include <nan.h>
#include "ActionHandler.h"
#include "EventHandler.h"
#include "ControlPanelHandler.h"

using namespace v8;

NAN_MODULE_INIT(function_init) {
//Action
	NAN_EXPORT(target, findAlljoynServices);
	NAN_EXPORT(target, updateBusName);
	NAN_EXPORT(target, getDeviceName);
	NAN_EXPORT(target, updateInterface);
	NAN_EXPORT(target, getInterfaceName);
	NAN_EXPORT(target, getActionName);
	NAN_EXPORT(target, doAction);

//Event  
	NAN_EXPORT(target, findAlljoynEventServices);
	NAN_EXPORT(target, updateEventBusName);
	NAN_EXPORT(target, getEventDeviceName);
	NAN_EXPORT(target, updateEventInterface);
	NAN_EXPORT(target, getEventInterfaceName);
	NAN_EXPORT(target, getEventName);
	NAN_EXPORT(target, doEvent);

//ControlPanel
	NAN_EXPORT(target, findControlPanelServices);
	NAN_EXPORT(target, getControlPanel);
	NAN_EXPORT(target, updateControlPanel);
	NAN_EXPORT(target, getControlPanelDeviceName);
	NAN_EXPORT(target, updateProperty);
	NAN_EXPORT(target, getProperty);
	NAN_EXPORT(target, setProperty);  
}

NODE_MODULE(node_alljoyn, function_init)
