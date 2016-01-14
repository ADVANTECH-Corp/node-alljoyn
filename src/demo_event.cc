/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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

#include <alljoyn/AboutData.h>
#include <alljoyn/AboutListener.h>
#include <alljoyn/AboutObjectDescription.h>
#include <alljoyn/AboutProxy.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Session.h>
#include <alljoyn/SessionListener.h>
#include <alljoyn/AllJoynStd.h>

#include <signal.h>
#include <stdio.h>
#include <vector>

/*
 * Note the removal of almost all Error handling to make the sample code more
 * straight forward to read.  This is only used here for demonstration actual
 * programs should check the return values of all method calls.
 */
using namespace ajn;

// The interface name should be the only thing required to find and form a
// connection between the service and the client using the about feature.
static const char* INTERFACE_NAME = "org.allseen.Introspectable";

BusAttachment* g_bus;

#define MAX_LENGTH 50

typedef struct EventMember {
    char method[32];
    char description[150];
} myEventMember;

typedef struct EventData {
    char path[64];
    char interface[32];
    myEventMember mam[MAX_LENGTH];
    size_t method_num;
} myEvents;

typedef struct EventAnnouncedData {
    char deviceName[32];
    char busName[32];
    SessionPort port;
    char path[MAX_LENGTH][64];
    size_t path_num;
} myEventAnnouncedData;

std::vector<myEventAnnouncedData> g_mad;
std::vector<myEvents> g_mac;
uint g_device_num = 0;
uint g_interface_num = 0;

class MySessionListener : public SessionListener {
    void SessionLost(SessionId sessionId, SessionLostReason reason) {
        printf("SessionLost sessionId = %u, Reason = %d\n", sessionId, reason);
    }
};

// Print out the fields found in the AboutData. Only fields with known signatures
// are printed out.  All others will be treated as an unknown field.
void printAboutData(AboutData& aboutData, const char* language, int tabNum)
{
    size_t count = aboutData.GetFields();

    const char** fields = new const char*[count];
    aboutData.GetFields(fields, count);

    for (size_t i = 0; i < count; ++i) {
        for (int j = 0; j < tabNum; ++j) {
            printf("\t");
        }
        printf("Key: %s", fields[i]);

        MsgArg* tmp;
        aboutData.GetField(fields[i], tmp, language);
        printf("\t");
        if (tmp->Signature() == "s") {
            const char* tmp_s;
            tmp->Get("s", &tmp_s);
            printf("%s", tmp_s);
        } else if (tmp->Signature() == "as") {
            size_t las;
            MsgArg* as_arg;
            tmp->Get("as", &las, &as_arg);
            for (size_t j = 0; j < las; ++j) {
                const char* tmp_s;
                as_arg[j].Get("s", &tmp_s);
                printf("%s ", tmp_s);
            }
        } else if (tmp->Signature() == "ay") {
            size_t lay;
            uint8_t* pay;
            tmp->Get("ay", &lay, &pay);
            for (size_t j = 0; j < lay; ++j) {
                printf("%02x ", pay[j]);
            }
        } else {
            printf("User Defined Value\tSignature: %s", tmp->Signature().c_str());
        }
        printf("\n");
    }
    delete [] fields;
}

char* GetXmlElementName(char* start_pch)
{
	if (start_pch[0] != '<')
		return NULL;
	if (start_pch[1] == '/')
		return NULL;

	char* pch = strstr(start_pch, "name=");
	if (pch == NULL)
		return NULL;

	char* end_pch = strchr(start_pch, '>');
	if (end_pch == NULL)
		return NULL;
	if (pch > end_pch)
		return NULL;
		
	size_t len = end_pch - pch - 7;
	char* tmpStr = new char[len+1];
	strncpy(tmpStr, pch + 6, len);
	tmpStr[len] = '\0';

	char* check_pch = strstr(tmpStr, "sessionless=");	
	if (check_pch != NULL){
		len = check_pch-tmpStr-2;
		tmpStr[len] = '\0';
	}
	
	return tmpStr;
}

char* GetXmlElementContent(char* start_pch, const char* target)
{
	if (start_pch[0] != '<')
		return NULL;
	if (start_pch[1] == '/')
		return NULL;

	size_t len = strlen(target);
	char* target_name = new char[len+2];
	sprintf(target_name, "<%s", target);
	target_name[len+1] = '\0';

	char* pch = strstr(start_pch, target_name);
	delete target_name;

	if (pch == NULL)
		return NULL;

	pch = strchr(pch, '>');
	if (*(pch-1) == '/')
		return NULL;

	char* target_end = new char[len+4];
	sprintf(target_end, "</%s>", target);
	target_end[len+3] = '\0';

	char* end_pch = strstr(pch, target_end);
	len = end_pch - pch - 1;
	delete target_end;

	if (len <= 0)
		return NULL;

	char* tmpStr = new char[len+1];
	strncpy(tmpStr, pch + 1, len);
	tmpStr[len] = '\0';

	return tmpStr;
}

char* ExtractXmlElement(char* start_pch)
{
	if (start_pch[0] != '<')
		return NULL;
	if (start_pch[1] == '/')
		return NULL;

	char* pch = strchr(start_pch, ' ');
	size_t len = pch - start_pch - 1;
	char* target_name = new char[len+1];
	strncpy(target_name, start_pch+1, len);
	target_name[len] = '\0';

	char* target_end = new char[len+4];
	sprintf(target_end, "</%s>", target_name);
	target_end[len+3] = '\0';

	char* end_pch = strstr(start_pch, target_end);
	len = len + 4;
	delete target_name;
	delete target_end;

	if (end_pch == NULL) {
		end_pch = strstr(start_pch, "/>");
		len = 3;
		if (end_pch == NULL)
			return NULL;
	}

	len = end_pch - start_pch + len;
	char* tmpStr = new char[len+1];
	strncpy(tmpStr, start_pch, len);
	tmpStr[len] = '\0';

	return tmpStr;
}

void ParseXmlforActionDescription(char* xml)
{
	//printf("IntrospectWithDescription: %s\n", xml);

	char* start_pch;
	char* end_pch;

	start_pch = strstr(xml, "<interface name=");
	if (start_pch)
		end_pch = strstr(xml, "</interface>");

	while (start_pch != NULL)
	{
		// Get interface
		char* tmpIntf = GetXmlElementName(start_pch);
		printf("\tinterface name=%s\n", tmpIntf);

		// Check description
		char* tmpStr = ExtractXmlElement(start_pch);

		start_pch = strstr(tmpStr, "<description ");
		if (start_pch)
		{
			printf("\t\thas description!\n");
			// Get action
			start_pch = strstr(tmpStr, "<signal name=");
			int i = 0;
			while (start_pch != NULL)
			{
        g_mac.push_back(myEvents());
				char* tmpMethod = GetXmlElementName(start_pch);
				printf("\t\tmethod name=%s\n", tmpMethod);

				// Get description
				char* tmpStr2 = ExtractXmlElement(start_pch);
				char* tmpDescription = GetXmlElementContent(tmpStr2, "description");
				printf("\t\tdescription: %s\n", tmpDescription);

				if (tmpDescription != NULL)
				{
					strcpy(g_mac[g_interface_num].mam[i].description, tmpDescription);
					strcpy(g_mac[g_interface_num].mam[i].method, tmpMethod);
					i++;
					g_mac[g_interface_num].method_num = i;
				}

				delete tmpMethod;
				delete tmpDescription;
				delete tmpStr2;

				// Check next method
				start_pch = strstr(start_pch + 8, "<signal name=");
			}

			if (i > 0)
			{
				char* tmpPath = GetXmlElementName(xml);
				strcpy(g_mac[g_interface_num].interface, tmpIntf);
				strcpy(g_mac[g_interface_num].path, tmpPath);
				g_interface_num++;
				delete tmpPath;
			}
		}
		delete tmpStr;
		delete tmpIntf;

		// Check next interface
		start_pch = strstr(end_pch, "<interface name=");
		if (start_pch)
			end_pch = strstr(start_pch, "</interface>");
	}
}

class MyEventAboutListener : public AboutListener {
    void Announced(const char* busName, uint16_t version, SessionPort port, const MsgArg& objectDescriptionArg, const MsgArg& aboutDataArg) {
	// #1: Get Device Name
	AboutData aboutData(aboutDataArg);
	char* deviceName;
	uint8_t* appId;
	size_t appId_num;
  g_mad.push_back(myEventAnnouncedData());

	if (aboutData.GetDeviceName(&deviceName) == ER_OK)
	{
		printf("DeviceName: %s\n", deviceName);
		strcpy(g_mad[g_device_num].deviceName, deviceName);
	} else {
		// If DeviceName does not exist, we use AppName & AppId
		aboutData.GetAppId(&appId, &appId_num);
		aboutData.GetAppName(&deviceName);
		printf("AppName + AppId: %s %2x%2x%2x%2x\n", deviceName, appId[12], appId[13], appId[14], appId[15]);
		sprintf(g_mad[g_device_num].deviceName, "%s %2x%2x%2x%2x", deviceName, appId[12], appId[13], appId[14], appId[15]);
	}

	strcpy(g_mad[g_device_num].busName, busName);
	g_mad[g_device_num].port = port;

	// #1-1: Get Path with "org.allseen.Introspectable"
	AboutObjectDescription aod(objectDescriptionArg);
	size_t path_num = aod.GetPaths(NULL, 0);
        const char** paths = new const char*[path_num];
        aod.GetPaths(paths, path_num);
        for (size_t i = 0; i < path_num; ++i) {
		size_t intf_num = aod.GetInterfaces(paths[i], NULL, 0);
		const char** intfs = new const char*[intf_num];
		aod.GetInterfaces(paths[i], intfs, intf_num);
		for (size_t j = 0; j < intf_num; ++j) {
			if (strcmp(intfs[j], "org.allseen.Introspectable") == 0) {
				printf("Find org.allseen.Introspectable in %s\n", paths[i]);
				size_t path_num = g_mad[g_device_num].path_num;
				strcpy(g_mad[g_device_num].path[path_num], paths[i]);
				g_mad[g_device_num].path_num++;
			}
		}
		delete [] intfs;
	}
	delete [] paths;

	g_device_num++;

	/*for (size_t i = 0; i < g_device_num; i++) {
		printf("Stored Device Name [%d]: %s\n", i, g_mad[i].deviceName);
		printf("\tbusName: %s\n", g_mad[i].busName);
		printf("\tport: %d\n", g_mad[i].port);
		for (size_t j = 0; j < g_mad[i].path_num; j++) {
			printf("\tpath[%d]: %s\n", j, g_mad[i].path[j]);
		}
	}*/

	// #2: Parse XML for description
	SessionId sessionId;
	SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
	g_bus->EnableConcurrentCallbacks();

	for (size_t i = 0; i < g_device_num; i++) {
		QStatus status = g_bus->JoinSession(g_mad[i].busName, g_mad[i].port, &sessionListener, sessionId, opts);
		printf("SessionJoined sessionId = %u, status = %s\n", sessionId, QCC_StatusText(status));

		if (ER_OK == status && 0 != sessionId) {
		for (size_t j = 0; j < g_mad[i].path_num; j++) {
			ProxyBusObject remoteObj(*g_bus, g_mad[i].busName, g_mad[i].path[j], sessionId);
			status = remoteObj.IntrospectRemoteObject();
			const char* ifcName = org::allseen::Introspectable::InterfaceName;

			const InterfaceDescription* introIntf = remoteObj.GetInterface(ifcName);
			if (!introIntf) {
				introIntf = g_bus->GetInterface(ifcName);
				remoteObj.AddInterface(*introIntf);
			}

			/* Attempt to retrieve introspection from the remote object using sync call */
			Message reply(*g_bus);
			const InterfaceDescription::Member* introMember = introIntf->GetMember("IntrospectWithDescription");

			MsgArg inputs[1];
			inputs[0].Set("s", "en");
			status = remoteObj.MethodCall(*introMember, inputs, 1, reply, 30000);

			/* Parse the XML reply */
			if (status == ER_OK) {
				ParseXmlforActionDescription((char*)reply->GetArg(0)->v_string.str);
			}
		}
		}
	}

	/*for (size_t i = 0; i < g_interface_num; i++) {
		printf("Stored Interface Name [%d]: %s\n", (int)i, g_mac[i].interface);
		printf("\tPath: %s\n", g_mac[i].path);
		for (size_t j = 0; j < g_mac[i].method_num; j++) {
			printf("\tmethod[%d]: %s => ", (int)j, g_mac[i].mam[j].method);
			printf("description:\"%s\"\n", g_mac[i].mam[j].description);
		}
	}*/

	// #3: Perform Action method call
	//int device_input = 0;
	//int interface_input = 0;
	//int method_input = 2;
	//QStatus status;

	/*SessionId sessionId;
	SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);

	QStatus status = g_bus->JoinSession(g_mad[device_input].busName, g_mad[device_input].port, &sessionListener, sessionId, opts);
	printf("SessionJoined sessionId = %u, status = %s\n", sessionId, QCC_StatusText(status));

	if (ER_OK == status && 0 != sessionId)*/
	/*{ 
		ProxyBusObject remoteObj(*g_bus, g_mad[device_input].busName, g_mac[interface_input].path, sessionId);
		status = remoteObj.IntrospectRemoteObject();

		const char* ifcName = g_mac[interface_input].interface;
		const InterfaceDescription* introIntf = remoteObj.GetInterface(ifcName);
		if (!introIntf) {
			introIntf = g_bus->GetInterface(ifcName);
			remoteObj.AddInterface(*introIntf);
		}

		Message reply(*g_bus);
		const char* methodName = g_mac[interface_input].mam[method_input].method;
		const InterfaceDescription::Member* introMember = introIntf->GetMember(methodName);
		status = remoteObj.MethodCall(*introMember, NULL, 0, reply, 30000);

		if (status == ER_OK) {
			printf("MethodCall: %s OK!\n", methodName);
		} else {
			printf("MethodCall: %s failed!\n", methodName);
		}
	}*/
    }
    MySessionListener sessionListener;
};

int main(int argc, char** argv)
{
    QStatus status;

    BusAttachment bus("AboutServiceTest", true);

    g_bus = &bus;

    status = bus.Start();
    if (ER_OK == status) {
        printf("BusAttachment started.\n");
    } else {
        printf("FAILED to start BusAttachment (%s)\n", QCC_StatusText(status));
        exit(1);
    }

    status = bus.Connect();
    if (ER_OK == status) {
        printf("BusAttachment connect succeeded.\n");
    } else {
        printf("FAILED to connect to router node (%s)\n", QCC_StatusText(status));
        exit(1);
    }

    MyEventAboutListener aboutListener;
    bus.RegisterAboutListener(aboutListener);

    const char* interfaces[] = { INTERFACE_NAME };
    status = bus.WhoImplements(interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    if (ER_OK == status) {
        printf("WhoImplements called.\n");
    } else {
        printf("WhoImplements call FAILED with status %s\n", QCC_StatusText(status));
        exit(1);
    }
    
    return 0;
}
