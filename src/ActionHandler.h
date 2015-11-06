#ifndef _ACTIONHANDLER_H
#define _ACTIONHANDLER_H

#include <nan.h>

NAN_METHOD(findAlljoynServices);
NAN_METHOD(updateBusName);
NAN_METHOD(getDeviceName);
NAN_METHOD(updateInterface);
NAN_METHOD(getInterfaceName);
NAN_METHOD(getActionName);
NAN_METHOD(doAction);

#endif /* _ACTIONHANDLER_H */
