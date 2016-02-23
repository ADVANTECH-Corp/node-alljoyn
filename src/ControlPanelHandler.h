#ifndef _CONTROLPANELHANDLER_H
#define _CONTROLPANELHANDLER_H

#include <nan.h>

NAN_METHOD(findControlPanelServices);
NAN_METHOD(getControlPanelDeviceName);
NAN_METHOD(updateControlPanel);
NAN_METHOD(getControlPanel);
NAN_METHOD(updateProperty);
NAN_METHOD(getProperty);
NAN_METHOD(setProperty);

#endif /* _CONTROLPANELHANDLER_H */
