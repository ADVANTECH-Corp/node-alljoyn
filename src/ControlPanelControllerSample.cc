/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <iostream>
#include <sstream>
#include <cstdio>
#include <signal.h>
//#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/ControlPanelController.h>
#include "ControlPanelListenerImpl.h"
//#include "ControllerNotificationReceiver.h"
#include <SrpKeyXListener.h>
#include <CommonSampleUtil.h>
#include <AnnounceHandlerImpl.h>
#include <alljoyn/services_common/LogModulesNames.h>

#define SERVICE_PORT 900

using namespace ajn;
using namespace services;
using namespace qcc;

typedef struct ContainerObject
{
	int index;
	qcc::String value;
	qcc::String label;
}ContainerObject;

BusAttachment* bus = 0;
ControlPanelService* controlPanelService = 0;
ControlPanelController* controlPanelController = 0;
ControlPanelListenerImpl* controlPanelListener = 0;
ControlPanelDevice* controlPanelDevice = 0;
SrpKeyXListener* srpKeyXListener = 0;
AnnounceHandlerImpl* announceHandler = 0;
//NotificationService* conService = 0;
//ControllerNotificationReceiver* receiver = 0;
qcc::String ControlPanelPrefix = "/ControlPanel/";
static volatile sig_atomic_t s_interrupt = false;

static void SigIntHandler(int sig)
{
    s_interrupt = true;
}

void cleanup()
{
    if (bus && announceHandler) {
        const char* interfaces[] = { "*" };
        bus->CancelWhoImplements(interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
        bus->UnregisterAboutListener(*announceHandler);
    }

    if (controlPanelService) {
        controlPanelService->shutdownController();
        delete controlPanelService;
        controlPanelService = NULL;
    }
    if (controlPanelController) {
        delete controlPanelController;
        controlPanelController = NULL;
    }
    if (controlPanelListener) {
        delete controlPanelListener;
        controlPanelListener = NULL;
    }
    if (announceHandler) {
        delete announceHandler;
        announceHandler = NULL;
    }
    if (srpKeyXListener) {
        delete srpKeyXListener;
        srpKeyXListener = NULL;
    }
    /*if (conService) {
        conService->shutdown();
        conService = NULL;
    }
    if (receiver) {
        delete receiver;
        receiver = NULL;
    }*/
    if (bus) {
        delete bus;
        bus = NULL;
    }
}

//std::map<qcc::String, unsigned short> MyPorts;
//std::map<qcc::String, qcc::String> MyDeviceNames;
//std::map<qcc::String, ControlPanelControllerUnit*> MyCPCU;
//std::multimap<qcc::String, ContainerObject> MyControlPanelData;
//std::multimap<qcc::String, ContainerObject> MyPropertyData;
//std::map<qcc::String, qcc::String> ControlPanelJansson; 

static void announceHandlerCallback(qcc::String const& busName, unsigned short version,
                                    unsigned short port, const ajn::AboutObjectDescription& objectDescription,
                                    const ajn::AboutData& aboutData)
{
    std::cout << "Got announceHandlerCallback" << std::endl;

    controlPanelDevice = controlPanelController->createControllableDevice(busName, objectDescription);    
/*

    // #1: Get Device Name which has ControllableDevice
    AboutData mad(aboutData);
    char* deviceName;
    char* appName;
    uint8_t* appId;
    size_t appId_num;

    if (mad.GetDeviceName(&deviceName) == ER_OK)
    {
	printf("[Test] DeviceName: %s\n", deviceName);
    } else {
	mad.GetAppName(&appName);
	mad.GetAppId(&appId, &appId_num);
	sprintf(deviceName, "%s %2x%2x%2x%2x", appName, appId[12], appId[13], appId[14], appId[15]);
	printf("[Test] AppName + AppId: %s\n", deviceName);
    }
    MyDeviceNames[busName] = qcc::String((const char*)deviceName, strlen(deviceName));
    MyPorts[busName] = port;

    // Debug Info
    std::map<qcc::String, unsigned short>::iterator it_port;
    for (it_port = MyPorts.begin() ; it_port != MyPorts.end() ; ++it_port)
	std::cout << "BusName: " << it_port->first << " ; Port: " << it_port->second << std::endl;

    std::map<qcc::String, qcc::String>::iterator it_dn;
     for (it_dn = MyDeviceNames.begin() ; it_dn != MyDeviceNames.end() ; ++it_dn)
        std::cout << "BusName: " << it_dn->first << " ; DeviceName: " << it_dn->second << std::endl;

*/

}

void WaitForSigInt(void) {
    while (s_interrupt == false) {
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }
}

int main()
{
    QStatus status;

    // Allow CTRL+C to end application
    signal(SIGINT, SigIntHandler);
    std::cout << "Beginning ControlPanel Application. (Press CTRL+C to end application)" << std::endl;

    // Initialize Service objects
#ifdef QCC_USING_BD
    PasswordManager::SetCredentials("ALLJOYN_PIN_KEYX", "000000");
#endif

    controlPanelService = ControlPanelService::getInstance();
    //Disable Debug by default
    QCC_SetDebugLevel(logModules::CONTROLPANEL_MODULE_LOG_NAME, 0);//logModules::ALL_LOG_LEVELS);

    srpKeyXListener = new SrpKeyXListener();

    bus = CommonSampleUtil::prepareBusAttachment(srpKeyXListener);
    if (bus == NULL) {
        std::cout << "Could not initialize BusAttachment." << std::endl;
        cleanup();
        return 1;
    }

    controlPanelController = new ControlPanelController();
    controlPanelListener = new ControlPanelListenerImpl(controlPanelController);

    status = controlPanelService->initController(bus, controlPanelController, controlPanelListener);
    if (status != ER_OK) {
        std::cout << "Could not initialize Controllee." << std::endl;
        cleanup();
        return 1;
    }

    announceHandler = new AnnounceHandlerImpl(NULL, announceHandlerCallback);
    bus->RegisterAboutListener(*announceHandler);

    const char* interfaces[] = { "*" };
    status = bus->WhoImplements(interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    if (ER_OK == status) {
        std::cout << "WhoImplements called." << std::endl;
    } else {
        std::cout << "ERROR - WhoImplements failed." << std::endl;
    }

    /*conService = NotificationService::getInstance();
    receiver = new ControllerNotificationReceiver(controlPanelController);
    status = conService->initReceive(bus, receiver);
    if (status != ER_OK) {
        std::cout << "Could not initialize receiver." << std::endl;
        cleanup();
        return 1;
    }*/

    std::cout << "Finished setup. Waiting for Contollees" << std::endl;

    WaitForSigInt();
    cleanup();

    return 0;
}


