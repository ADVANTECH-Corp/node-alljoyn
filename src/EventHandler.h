#ifndef _EVENTHANDLER_H
#define _EVENTHANDLER_H

#include <nan.h>

NAN_METHOD(findAlljoynEventServices);
NAN_METHOD(updateEventBusName);
NAN_METHOD(getEventDeviceName);
NAN_METHOD(updateEventInterface);
NAN_METHOD(getEventInterfaceName);
NAN_METHOD(getEventName);
NAN_METHOD(doEvent);

#endif /* _EVENTHANDLER_H */
