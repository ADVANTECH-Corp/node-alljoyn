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
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/ControlPanelController.h>
#include "ControlPanelListenerImpl.h"
#include <SrpKeyXListener.h>
#include <CommonSampleUtil.h>
#include <AnnounceHandlerImpl.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include "ControlPanelHandler.h"

#define SERVICE_PORT 900
#define STATUS_ERR -1

using namespace ajn;
using namespace services;
using namespace qcc;

typedef struct ContainerObject
{
	int index;
	qcc::String value;
	qcc::String label;
}ContainerObject;

typedef struct PropertyObject
{
	bool is_Writable;
  bool has_range;
  bool has_list;
	int index;
	qcc::String value;
	qcc::String label;
  std::vector<uint16_t> hints;
  ajn::services::PropertyType propertyType;
 	uint16_t min_value;
  uint16_t max_value;
  std::vector<ajn::services::ConstraintList> constraintList;
}PropertyObject;

typedef struct AnnouncedData {
    char deviceName[32];
    char appName[32];
    char busName[32];
    char path[64];
    char interface[32];    
		uint8_t appId[16];
		ajn::SessionPort port;
		uint32_t runLevel;
		uint32_t index;
		qcc::String WriteJassonString;		
} myAnnouncedData;

typedef std::multimap<qcc::String, ContainerObject> CONTAINER_TYPE;
typedef std::multimap<qcc::String, PropertyObject> PROPERTY_TYPE;
	
BusAttachment* bus = 0;
ControlPanelService* controlPanelService = 0;
ControlPanelController* controlPanelController = 0;
ControlPanelListenerImpl* controlPanelListener = 0;
ControlPanelDevice* controlPanelDevice = 0;
SrpKeyXListener* srpKeyXListener = 0;
AnnounceHandlerImpl* announceHandler = 0;
qcc::String ControlPanelPrefix = "/ControlPanel/";

std::map<uint32_t, myAnnouncedData> MyDevice;

extern CONTAINER_TYPE MyControlPanelData;
extern PROPERTY_TYPE MyPropertyData;
//extern std::map<qcc::String, qcc::String> ControlPanelJansson; 
extern qcc::String JanssonString; 
extern myAnnouncedData nowAnnouncedData;

static void announceHandlerCallback(qcc::String const& busName, unsigned short version,
                                    unsigned short port, const ajn::AboutObjectDescription& objectDescription,
                                    const ajn::AboutData& aboutData)
{	
    myAnnouncedData temp_announceData;
    
    char* deviceName;
    char* appName;
    uint8_t* appId;
    size_t appId_num;  

    uint32_t index = MyDevice.size();
		AboutData v_aboutData(aboutData);
		
    v_aboutData.GetAppId(&appId, &appId_num);
    memcpy(temp_announceData.appId, appId, appId_num);

    v_aboutData.GetAppName(&appName);
    strcpy(temp_announceData.appName, appName);

    if (v_aboutData.GetDeviceName(&deviceName) == ER_OK){
			std::cout <<"[ControlPanelHandler-announceHandlerCallback] DeviceName:" << deviceName << std::endl;
    } else {
			v_aboutData.GetAppName(&appName);
			v_aboutData.GetAppId(&appId, &appId_num);
			sprintf(deviceName, "%s %2x%2x%2x%2x", appName, appId[12], appId[13], appId[14], appId[15]);
			std::cout << "[ControlPanelHandler-announceHandlerCallback] AppName + AppId:" << deviceName << std::endl;
    }
    
    strcpy(temp_announceData.deviceName, deviceName);
    strcpy(temp_announceData.busName, busName.c_str());
    temp_announceData.port = port;
		temp_announceData.runLevel = 0; // First, need to 0
		MyDevice.insert(std::pair<uint32_t, myAnnouncedData>(index, temp_announceData));
		
	//Start Session
	controlPanelController->createControllableDevice(busName, objectDescription);
    //do jobs in createControllableDevice()

}

NAN_METHOD(setProperty)
{
		NanScope();
		QStatus status = ER_OK;	

		if (args.Length() < 1) {
        NanReturnValue(NanNew<v8::Number>((int) STATUS_ERR));
    } 

	  v8::String::Utf8Value Utf8Value_Jasson(args[0]->ToString());
	  const char *char_Jasson = *Utf8Value_Jasson;
		
		nowAnnouncedData.runLevel = 3;
		nowAnnouncedData.index = -1;
		nowAnnouncedData.WriteJassonString.assign(char_Jasson);
	
		status = controlPanelDevice->startSession();
		if (ER_OK != status){
				std::cout << "Could not get ControlPanelDevice." << QCC_StatusText(status) << std::endl;
				NanReturnValue(NanNew<v8::String>("N/A"));
		}else{
     		 
         NanReturnValue(NanNew<v8::String>(JanssonString.c_str()));
    }
}

NAN_METHOD(updateProperty)
{
		NanScope();
		QStatus status = ER_OK;	

		if (args.Length() < 1) {
        NanReturnValue(NanNew<v8::Number>((int) STATUS_ERR));
    } 

    uint32_t CP_index = args[0]->Uint32Value();
		
		nowAnnouncedData.runLevel = 2;
		nowAnnouncedData.index = CP_index;
		
		status = controlPanelDevice->startSession();
		if (ER_OK != status){
				std::cout << "Could not get ControlPanelDevice." << QCC_StatusText(status) << std::endl;
        NanReturnValue(NanNew<v8::Number>((int) STATUS_ERR));
		}else{
        NanReturnValue(NanNew<v8::Number>((int)status));
    }
}

NAN_METHOD(getProperty)
{
		NanScope();
    int num_PropertyData = MyPropertyData.size();
		
    if (args.Length() < 1) {
        NanReturnValue(NanNew<v8::Number>((int) num_PropertyData));
    }
    uint32_t CP_index = args[0]->Uint32Value();
    if (CP_index == nowAnnouncedData.index)
    {
        if (num_PropertyData < 1) {
            NanReturnValue(NanNew<v8::String>("N/A"));
     		}else{    
         		NanReturnValue(NanNew<v8::String>(JanssonString.c_str()));
        }
    }else
    {
        NanReturnValue(NanNew<v8::Number>((int) STATUS_ERR));
    }
		
}

NAN_METHOD(updateControlPanel)
{
		NanScope();
		QStatus status = ER_OK;
    
    if (args.Length() < 1) {
        NanReturnValue(NanNew<v8::Number>((int) STATUS_ERR));
    }
    
    //If no parameter, we return the size of device name.
    //Otherwise, we return the device name of index.
    uint32_t dev_index = args[0]->Uint32Value();
    std::map<uint32_t, myAnnouncedData>::iterator ret_iter;
       
		ret_iter = MyDevice.find(dev_index);
		//std::cout << "[ControlPanelHandler-updateControlPanel] select Dev: " << ret_iter->second.deviceName << std::endl;
		
		nowAnnouncedData = ret_iter->second;
		nowAnnouncedData.runLevel = 1;

		controlPanelDevice = controlPanelController->getControllableDevice(ret_iter->second.busName);	

		//Start Session
		status = controlPanelDevice->startSession();
		if (ER_OK != status)
			std::cout << "Could not get ControlPanelDevice." << QCC_StatusText(status) << std::endl;

    int num_ControlPanel = MyControlPanelData.size();		
    if (num_ControlPanel < 0)
      NanReturnValue(NanNew<v8::Number>((int) STATUS_ERR));
    else   
      NanReturnValue(NanNew<v8::Number>((int)status));
}

NAN_METHOD(getControlPanel)
{
		NanScope();
    
    if (args.Length() < 1) {
        NanReturnValue(NanNew<v8::Number>( MyControlPanelData.size()));
    } else {        
        	  uint32_t total = 0;
        	  uint32_t temp = 0;
        	      
            uint32_t CP_index = args[0]->Uint32Value();
            ContainerObject Select_ControlPanel;
        		CONTAINER_TYPE::iterator it_ControlPanelData;
        		
        		total = MyControlPanelData.size();
        		if (CP_index < total){	 		
    		    		for (it_ControlPanelData = MyControlPanelData.begin() ; it_ControlPanelData != MyControlPanelData.end() ; ++it_ControlPanelData)
    		    		{
    		    				temp = it_ControlPanelData->second.index;
    		        		if (temp == CP_index){
    		        				Select_ControlPanel = it_ControlPanelData->second;
    		        				nowAnnouncedData.index = CP_index;
    		        				
    		        				/* Debug */
    		        				/*
    		        				std::cout << "[Select] " << it_ControlPanelData->first << " , " << it_ControlPanelData->second.index<<"," \
    								    << it_ControlPanelData->second.value <<","<< it_ControlPanelData->second.label<< std::endl;		
    		        	      */
    		        	  } 
    		        }
            		
                temp = Select_ControlPanel.index;
            		if (temp == CP_index){
                    NanReturnValue(NanNew<v8::String>(Select_ControlPanel.value.c_str()));
                }else{
                    NanReturnValue(NanNew<v8::String>("N/A"));
                }
            }else
            {
                NanReturnValue(NanNew<v8::Number>((int) STATUS_ERR));
            }        
    }
}

NAN_METHOD(getControlPanelDeviceName)
{
    NanScope();
    
    //If no parameter, we return the size of device name.
    //Otherwise, we return the device name of index.
    std::map<uint32_t, myAnnouncedData>::iterator ret_iter;
    uint32_t total = 0;
    
    total = MyDevice.size();
    if (args.Length() < 1)
    {
        NanReturnValue(NanNew<v8::Number>(total));
    } 
    else 
    {
        uint32_t index = args[0]->Uint32Value();
		    if (index < total)
		    {
		    		ret_iter = MyDevice.find(index);
		        NanReturnValue(NanNew<v8::String>(ret_iter->second.deviceName));
		    
		    }else
		        NanReturnValue(NanNew<v8::String>("N/A"));
    }
}

void ControlPanelResetDevice() {
  MyDevice.clear();
}

void ControlPanelResetIntf() {
  MyControlPanelData.clear();
  MyPropertyData.clear(); 
  JanssonString.clear();
}

void ControlPanel_Bus_Deinit() {
    /* Deallocate bus */
    if (bus)
        delete bus;
    bus = NULL;
}

void ControlPanel_Bus_Init() {
    /* Create message bus */
    ControlPanel_Bus_Deinit();

    bus = CommonSampleUtil::prepareBusAttachment(srpKeyXListener);
}

NAN_METHOD(findControlPanelServices)
{	        
    NanScope();

    QStatus status = ER_OK;

    /* Reset status */
    ControlPanelResetDevice();
    ControlPanelResetIntf();
    
    controlPanelService = ControlPanelService::getInstance();
    srpKeyXListener = new SrpKeyXListener();

	  ControlPanel_Bus_Init();
		
    if (!bus) {
        status = ER_OUT_OF_MEMORY;
        std::cout << "FAILED to create BusAttachment." << QCC_StatusText(status) << std::endl;
    }    
 
    controlPanelController = new ControlPanelController();

    controlPanelListener = new ControlPanelListenerImpl(controlPanelController);

    status = controlPanelService->initController(bus, controlPanelController, controlPanelListener);
    if (ER_OK != status) {
        std::cout << "Could not initialize Controllee." << QCC_StatusText(status) << std::endl;
    }    
    
    announceHandler = new AnnounceHandlerImpl(NULL, announceHandlerCallback);
    bus->RegisterAboutListener(*announceHandler);

    const char* interfaces[] = { "*" };
    status = bus->WhoImplements(interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    if (ER_OK == status) {
        std::cout << "WhoImplements called." << QCC_StatusText(status) << std::endl;
    } else {
        std::cout << "ERROR - WhoImplements failed." << QCC_StatusText(status) << std::endl;
    }

    std::cout << "Finished setup. Waiting for Contollees" << QCC_StatusText(status) << std::endl;

    /* Perform the service asynchronously for 3 seconds */
    if (ER_OK == status) {
        unsigned int count = 0;
        while (count < 30) {
            count++;
#ifdef _WIN32
            Sleep(100);
#else
            usleep(100 * 1000);
#endif
        }
        bus->CancelWhoImplements(interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
        bus->UnregisterAboutListener(*announceHandler);
    }
    else
    {
    	ControlPanel_Bus_Deinit();
    }
    
    NanReturnValue(NanNew<v8::Number>((int) status));
}