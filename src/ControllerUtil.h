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

#ifndef CONTROLLERUTIL_H_
#define CONTROLLERUTIL_H_

#include <alljoyn/controlpanel/Container.h>
#include <alljoyn/controlpanel/Property.h>
#include <alljoyn/controlpanel/Dialog.h>
#include <alljoyn/controlpanel/Action.h>

#define PROPERTY_HINTS_SIZE 14

/**
 * Class used to simulate accessing of widgets, executing actions setting properties etc
 */

#define PROPERTY_JASSON_SIZE 9
typedef struct ContainerObject
{
	int index;
	qcc::String value;
	qcc::String label;
}ContainerObject;

typedef struct PropertyObject
{
	bool is_writable;
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

typedef std::multimap<qcc::String, ContainerObject> CONTAINER_MULTIMAP;
typedef std::multimap<qcc::String, PropertyObject> PROPERTY_MULTIMAP;
	
class ControllerUtil {

  public:

    ControllerUtil();

    virtual ~ControllerUtil();

    static void printRootWidget(ajn::services::RootWidget* rootWidget);

    static void printPropertyValue(ajn::services::PropertyValue propertyValue, ajn::services::PropertyType propertyType, qcc::String const& indent = "  ");

  private:
  	
  	//Advantech
		static void ListValue(ajn::services::Container* container, PROPERTY_MULTIMAP &MySTL);
		static void ListValue(ajn::services::Property* property, PROPERTY_MULTIMAP &MySTL);
		static void ListContainer(ajn::services::Container* rootContainer, CONTAINER_MULTIMAP &MySTL);
		static ajn::services::Widget* LocateContainer(ajn::services::Container* container, size_t index);
		static qcc::String PROPERTY_JASSON_NAMES[PROPERTY_JASSON_SIZE];
    ///////////////////////////////////
    
    static void printErrorWidget(ajn::services::Widget* widget, qcc::String const& indent);

    static void printBasicWidget(ajn::services::Widget* widget, qcc::String const& widgetType, qcc::String const& indent);

    static void printContainer(ajn::services::Container* rootContainer, std::vector<ajn::services::Action*>& actionsToExecute,
                               std::vector<ajn::services::Dialog*>& dialogsToExecute, std::vector<ajn::services::Property*>& propertiesToChange,
                               qcc::String const& indent = "");

    static void printProperty(ajn::services::Property* property, qcc::String const& indent);

    static void printDialog(ajn::services::Dialog* dialog, qcc::String const& indent);

    static void printConstraintRange(ajn::services::ConstraintRange* constraintRange, ajn::services::PropertyType propertyType,
                                     qcc::String const& indent);

    static void printConstraintList(const std::vector<ajn::services::ConstraintList>& constraintList, ajn::services::PropertyType propertyType,
                                    qcc::String const& indent);

    static void executeActions(std::vector<ajn::services::Action*>& actionsToExecute);

    static void executeDialogActions(std::vector<ajn::services::Dialog*>& dialogsToExecute);

    static void setPropertyValues(std::vector<ajn::services::Property*>& propertiesToChange);

    static void printHints(ajn::services::Widget* widget, qcc::String const& widgetType, qcc::String const& indent);

    static qcc::String PROPERTY_HINTS_STRINGS[PROPERTY_HINTS_SIZE];
    
};

#endif /* CONTROLLERUTIL_H_ */
