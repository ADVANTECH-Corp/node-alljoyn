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

#include "ControllerUtil.h"
#include <alljoyn/controlpanel/ControlPanel.h>
#include <alljoyn/controlpanel/ActionWithDialog.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>

using namespace ajn;
using namespace services;

qcc::String ControllerUtil::PROPERTY_HINTS_STRINGS[PROPERTY_HINTS_SIZE] = { "", "SWITCH", "CHECKBOX", "SPINNER", "RADIOBUTTON", "SLIDER",
                                                                            "TIMEPICKER", "DATEPICKER", "NUMBERPICKER", "KEYPAD", "ROTARYKNOB",
                                                                            "TEXTVIEW", "NUMERICVIEW", "EDITTEXT" };

qcc::String PROPERTY_JASSON_NAMES[PROPERTY_JASSON_SIZE] = { "index", "name", "value", "writable" ,"hints", "type", "min", "max", "list"};

ControllerUtil::ControllerUtil()
{

}

ControllerUtil::~ControllerUtil()
{

}


CONTAINER_MULTIMAP MyControlPanelData;
PROPERTY_MULTIMAP MyPropertyData;
qcc::String JanssonString;
//std::map<qcc::String, qcc::String> ControlPanelJansson; 
myAnnouncedData nowAnnouncedData;
Container* NowContainer = NULL;
Property* NowProperty = NULL;

template <typename T>
void erase_ObjectSTL(T &MySTL)
{
		if (MySTL.size() > 0)
		{
			for (typename T::iterator iter = MySTL.begin(); iter != MySTL.end();)
			{
			    // removes all values
			    typename T::iterator erase_iter = iter++; 
					MySTL.erase(erase_iter);
			}
		}
}
/*
void erase_StringMap(std::map<qcc::String, qcc::String> &MyMap)
{
		if (MyMap.size() > 0)
		{
			for (std::map<qcc::String, qcc::String>::iterator iter = MyMap.begin(); iter != MyMap.end();)
			{
			    // removes all values
			    std::map<qcc::String, qcc::String>::iterator erase_iter = iter++; 
					MyMap.erase(erase_iter);
			}
		}
}
*/
template <typename T>
typename T::iterator FindLink(qcc::String target, T &MySTL)
{
 	//Find all values by key using count & find 
  typename T::size_type entries = MySTL.count(target);
  typename T::iterator ret_iter = MySTL.find(target);
  
  /* Debug */
  /*
  for(T::size_type cnt = 0; cnt != entries; ++cnt, ++ret_iter) 
  	std::cout << "-----"<<ret_iter->first << ", "<< ret_iter->second << std::endl;
	ret_iter = MySTL.find(target);	
	*/
	return ret_iter;
}
qcc::String CovertJanssonFormat(PROPERTY_MULTIMAP::iterator Data)
{
		std::stringstream sstring;

		sstring << "{";  	
    for(int i= 0; i < PROPERTY_JASSON_SIZE; i++)
    {
				if ((i == 6 || i == 7 ) && (Data->second.has_range == false)){
						continue;
				}else if ((i == 8) && (Data->second.has_list == false)){
						continue;
				}
				
				sstring << "\""<< PROPERTY_JASSON_NAMES[i].c_str() <<"\":";
				if (i != 8)
					sstring << "\"";
					
				switch(i){
					case 0:
								sstring << Data->second.index <<"\",";
								break;
					case 1:
								sstring << Data->second.label <<"\",";
								break;
				 	case 2:
								sstring << Data->second.value <<"\",";
								break;
					case 3:
								sstring << Data->second.is_writable <<"\",";	
								break;		
					case 4:
								sstring << Data->second.hints[0] <<"\",";
								break;
					case 5:
								sstring << Data->second.propertyType <<"\"";
								if ((Data->second.has_range == true) || (Data->second.has_list == true)) 
									sstring <<",";
								break;
				 	case 6:
				 		    if (Data->second.has_range == true)
				 		    	sstring << Data->second.min_value <<"\",";
								break;
					case 7:
								if (Data->second.has_range == true)
									sstring << Data->second.max_value <<"\"";						
								if (Data->second.has_list == true) 
									sstring <<",";
								break;
	
					case 8: // array style
								if (Data->second.has_list == true){
									//sstring << Data->second.constraintList.size() <<"\",";
									sstring << "[";
									for (size_t i = 0; i < Data->second.constraintList.size(); i++)
									{
									
											sstring << "{\"Display\":\"" << Data->second.constraintList[i].getDisplay().c_str() <<"\",";
        							switch (Data->second.propertyType) {
        									case UINT16_PROPERTY:
            									sstring << "\"Value\":\"" << Data->second.constraintList[i].getConstraintValue().uint16Value <<"\"";
														break;
													default:
														std::cout << "Property type error." << std::endl;
														break;
											}
											sstring << "}";
											if (i < (Data->second.constraintList.size() - 1))
												sstring <<",";
									}
									sstring << "]";
							  }
								break;
					default:
								break;								
				}
		}    	
		sstring << "}";
		/* Debug */	
		//std::cout<< sstring.str().c_str() << std::endl;
		
		return sstring.str().c_str();
}

void GenJansson()
{
  qcc::String Target="ControlPanel";
	CONTAINER_MULTIMAP::iterator ContainerIterator;  
	CONTAINER_MULTIMAP::iterator PropertyIterator;
	PROPERTY_MULTIMAP::iterator ValueIterator;
	
	//Find Container	
	ContainerIterator = FindLink<CONTAINER_MULTIMAP>(Target, MyControlPanelData);
	
	CONTAINER_MULTIMAP::size_type count_1 = MyControlPanelData.count(Target);
  CONTAINER_MULTIMAP::size_type count_2;
  PROPERTY_MULTIMAP::size_type count_3;
  				
  for(CONTAINER_MULTIMAP::size_type cnt = 0; cnt != count_1; ++cnt, ++ContainerIterator) 
  {
	  	if (cnt == nowAnnouncedData.index){
			  	JanssonString="";
			  	//Find Property
			  	Target=ContainerIterator->second.value;
			    count_2 = MyControlPanelData.count(Target);
			  	PropertyIterator = FindLink<CONTAINER_MULTIMAP>(Target, MyControlPanelData);  
			
					JanssonString.append("{");
			   	for(CONTAINER_MULTIMAP::size_type cnt2 = 0; cnt2 != count_2; ++cnt2, ++PropertyIterator) 
			  	{   	
				  		//Find Label & Value
				  		qcc::String Temp_JanssonString="";
				  		Target=PropertyIterator->second.value;
				  		count_3 = MyPropertyData.count(Target);
					  	ValueIterator = FindLink<PROPERTY_MULTIMAP>(Target, MyPropertyData);
					  	
					  	Temp_JanssonString = CovertJanssonFormat(ValueIterator);
					  	JanssonString.append(Temp_JanssonString);
					  	if (cnt2 != (count_2-1))
					  		JanssonString.append(",");
			  	}
			  	JanssonString.append("}");
			  	//ControlPanelJansson.insert(std::pair<qcc::String, qcc::String>(ContainerIterator->second.value, JanssonString));
					
					/*Debug*/
					//std::cout<< JanssonString << std::endl;
			}else{
				continue;
			}
	}
}

qcc::String StringPropertyValue(PropertyValue propertyValue, PropertyType propertyType)
{
		qcc::String ret;
		std::stringstream convert;

	  switch (propertyType) {
    case UINT16_PROPERTY:
        convert << propertyValue.uint16Value;
        break;

    case INT16_PROPERTY:
    		convert << propertyValue.int16Value;
        break;

    case UINT32_PROPERTY:
        convert << propertyValue.uint32Value;
        break;

    case INT32_PROPERTY:
        convert << propertyValue.int32Value;
        break;

    case UINT64_PROPERTY:
    		convert << propertyValue.uint64Value;
        break;

    case INT64_PROPERTY:
    		convert << propertyValue.int64Value;
        break;

    case DOUBLE_PROPERTY:
    		convert << propertyValue.doubleValue;
        break;

    case STRING_PROPERTY:
    		convert << propertyValue.charValue;
        break;

    case BOOL_PROPERTY:
    		convert << (propertyValue.boolValue ? "true" : "false");
        break;

    case DATE_PROPERTY:
        convert << propertyValue.dateValue->getDay() << "/" <<
        propertyValue.dateValue->getMonth() << "/" <<
        propertyValue.dateValue->getYear();
        break;

    case TIME_PROPERTY:
        convert << propertyValue.timeValue->getHour() << ":" <<
        propertyValue.timeValue->getMinute() << ":" <<
        propertyValue.timeValue->getSecond();
        break;

    default:
        std::cout << "Property is unknown property type" << std::endl;
        break;
    }
 	
    ret = convert.str().c_str();
    return ret.c_str();
}

void ParserJasson(qcc::String JassonString, std::vector<uint32_t> &MyVector)
{
		const char* delim = "\"{}:,; ";
		char * pch;
		uint32_t i;
		pch = strtok((char*)JassonString.c_str(), delim);
		while (pch != NULL){
			if ( strcmp(pch,"index") != 0 && strcmp(pch,"value") != 0){// != "index" && !="value" 
				i = atoi(pch);
				MyVector.push_back(i);
			}
			pch = strtok (NULL, delim);
		}
}
	
void ControllerUtil::printRootWidget(RootWidget* rootWidget)
{
    if (!rootWidget) {
        return;
    }

    std::vector<Action*> actionsToExecute;
    std::vector<Property*> propertiesToChange;
    std::vector<Dialog*> dialogsToExecute;

    if (rootWidget->getWidgetType() == WIDGET_TYPE_CONTAINER){
				std::cout << "[Test] levle: " << nowAnnouncedData.runLevel << ", index: " << nowAnnouncedData.index << std::endl;
				if (nowAnnouncedData.runLevel == 1){
						erase_ObjectSTL<CONTAINER_MULTIMAP>(MyControlPanelData);
				    
				    // List Containers, and Locate the Container
				    ListContainer((Container*)rootWidget, MyControlPanelData);
				}
				else if (nowAnnouncedData.runLevel == 2){
						erase_ObjectSTL<PROPERTY_MULTIMAP>(MyPropertyData);
						NowContainer = (Container*)LocateContainer((Container*)rootWidget, nowAnnouncedData.index);
		
						//erase_StringMap(ControlPanelJansson);
				    // List Containers, and Locate the Property
			      ListContainer(NowContainer, MyControlPanelData);
				    // List value of Container
						ListValue(NowContainer, MyPropertyData);
						GenJansson();									
				}else if (nowAnnouncedData.runLevel == 3){ //Set
			      // Parser SET String function
			      QStatus status = ER_FAIL ;
			      if (nowAnnouncedData.WriteJassonString.size() != 0){
					      uint32_t index, new_value, orig_value;
					      std::vector<uint32_t> vec_WritePropertyData;
					      //Covert Jasson to qcc::String	
					      ParserJasson(nowAnnouncedData.WriteJassonString, vec_WritePropertyData);
					      for (uint32_t i = 0 ; i < vec_WritePropertyData.size(); i+=2){
					      	 	index = vec_WritePropertyData[i];
					      	 	new_value = vec_WritePropertyData[i+1];
					      	 	// List value of Property
					      	 	NowProperty = (Property*)LocateContainer(NowContainer, (size_t)index);
										if (NowProperty->getPropertyType() == UINT16_PROPERTY){
											orig_value = NowProperty->getPropertyValue().uint16Value;										
						      	 	if (NowProperty->getIsWritable() && new_value != orig_value){
										      //ListValue(NowProperty, MyPropertyData);
									       status = NowProperty->setValue((uint16_t)new_value);
							      	}
										}else{
											status = ER_WRITE_ERROR  ;
										}
										if (status != ER_OK)
											std::cout << "Setting Property Failed." << std::endl;
					      }
			      	
			      }
				}
		
		    /* Debug */
		    /*
		    CONTAINER_MULTIMAP::iterator it_ControlPanelData;
		    for (it_ControlPanelData = MyControlPanelData.begin() ; it_ControlPanelData != MyControlPanelData.end() ; ++it_ControlPanelData)
		        std::cout << "[MAP1] " << it_ControlPanelData->first << " , " << it_ControlPanelData->second.index<<"," \
						<< it_ControlPanelData->second.value <<","<< it_ControlPanelData->second.label<< std::endl;
				
		    CONTAINER_MULTIMAP::iterator it_PropertyData;
		    for (it_PropertyData = MyPropertyData.begin() ; it_PropertyData != MyPropertyData.end() ; ++it_PropertyData)
		        std::cout << "[MAP2] " << it_PropertyData->first << " , " << it_PropertyData->second.index<<"," \
		        	<< it_PropertyData->second.value <<","<< it_PropertyData->second.label<< std::endl;
				*/

/*              
        printContainer((Container*)rootWidget, actionsToExecute, dialogsToExecute, propertiesToChange);
        
        std::cout << "Going to execute all Actions now" << std::endl;
        executeActions(actionsToExecute);
        std::cout << std::endl;

        std::cout << "Going to execute all Dialog Actions now" << std::endl;
        executeDialogActions(dialogsToExecute);
        std::cout << std::endl;

        std::cout << "Going to call setValue for all Properties now" << std::endl;
        setPropertyValues(propertiesToChange);
        std::cout << std::endl;
*/
    } else if (rootWidget->getWidgetType() == WIDGET_TYPE_DIALOG) {

        printDialog((Dialog*)rootWidget, "");
        dialogsToExecute.push_back((Dialog*)rootWidget);

        std::cout << "Going to execute all Dialog Actions now" << std::endl;
        executeDialogActions(dialogsToExecute);
        std::cout << std::endl;

    } else {
        std::cout << "RootWidget is of unknown type." << std::endl;
    }
}

Widget* ControllerUtil::LocateContainer(Container* container, size_t index)
{
	std::vector<Widget*> childWidgets = container->getChildWidgets();
	
	return childWidgets[index];
	
}
void ControllerUtil::ListContainer(Container* container, CONTAINER_MULTIMAP &MySTL)
{
		ContainerObject temp_object;
    std::vector<Widget*> childWidgets = container->getChildWidgets();
    
    for (size_t i = 0; i < childWidgets.size(); i++) 
    {
    		temp_object.index = i;
        temp_object.value = childWidgets[i]->getWidgetName().c_str();               			   		
    		
    		MySTL.insert(std::pair<qcc::String, ContainerObject>(container->getWidgetName().c_str(), temp_object));
    }
}

void ControllerUtil::ListValue(Container* container, PROPERTY_MULTIMAP &MySTL)
{  
    std::vector<Widget*> childWidgets = container->getChildWidgets();
 	  WidgetType widgetType = container->getWidgetType();
    if (WIDGET_TYPE_CONTAINER == widgetType)
    {
	    for (size_t i = 0; i < childWidgets.size(); i++) {
	    	ListValue((Property*)childWidgets[i], MySTL);
	    }
		}
}

void ControllerUtil::printContainer(Container* container, std::vector<Action*>& actionsToExecute, std::vector<Dialog*>& dialogsToExecute,
                                    std::vector<Property*>& propertiesToChange, qcc::String const& indent)
{
    printBasicWidget(container, "Container", indent);

    std::vector<Widget*> childWidgets = container->getChildWidgets();
    std::cout << indent.c_str() << "Print ChildWidgets: " << std::endl;
    for (size_t i = 0; i < childWidgets.size(); i++) {
        //MyControlPanelData.insert(std::pair<qcc::String, qcc::String>(container->getWidgetName().c_str(), childWidgets[i]->getWidgetName().c_str()));
        WidgetType widgetType = childWidgets[i]->getWidgetType();
        switch (widgetType) {
        case WIDGET_TYPE_ACTION:
            printBasicWidget(childWidgets[i], "Action", indent + "  ");
            actionsToExecute.push_back((Action*)childWidgets[i]);
            break;

        case WIDGET_TYPE_ACTION_WITH_DIALOG:
            printBasicWidget(childWidgets[i], "Action", indent + "  ");
            std::cout << indent.c_str() << "  Printing ChildDialog: " << std::endl;
            printDialog(((ActionWithDialog*)childWidgets[i])->getChildDialog(), indent + "    ");
            dialogsToExecute.push_back(((ActionWithDialog*)childWidgets[i])->getChildDialog());
            break;

        case WIDGET_TYPE_LABEL:
            printBasicWidget(childWidgets[i], "Label", indent + "  ");
            break;

        case WIDGET_TYPE_PROPERTY:
            printProperty(((Property*)childWidgets[i]), indent + "  ");
            propertiesToChange.push_back((Property*)childWidgets[i]);
            break;

        case WIDGET_TYPE_CONTAINER:
            printContainer(((Container*)childWidgets[i]), actionsToExecute, dialogsToExecute, propertiesToChange, indent + "  ");
            break;

        case WIDGET_TYPE_DIALOG:
            printDialog(((Dialog*)childWidgets[i]), indent + "  ");
            break;

        case WIDGET_TYPE_ERROR:
            printErrorWidget(childWidgets[i], indent + "  ");
            break;
        }
    }    
    
}

void ControllerUtil::printProperty(Property* property, qcc::String const& indent)
{
    printBasicWidget(property, "Property", indent);
    printPropertyValue(property->getPropertyValue(), property->getPropertyType(), indent);
    std::cout << indent.c_str() << "Property is " << (property->getIsWritable() ? "writable" : "not writable") << std::endl;

    if (property->getUnitOfMeasure().size()) {
        std::cout << indent.c_str() << "Property unitOfMeasure: " << property->getUnitOfMeasure().c_str() << std::endl;
    }
    if (property->getConstraintRange()) {
        std::cout << indent.c_str() << "Property has ConstraintRange: " << std::endl;
        printConstraintRange(property->getConstraintRange(), property->getPropertyType(), indent + "  ");
    }
    if (property->getConstraintList().size()) {
        std::cout << indent.c_str() << "Property has ConstraintList: " << std::endl;
        printConstraintList(property->getConstraintList(), property->getPropertyType(), indent + "  ");
    }
}

void ControllerUtil::printDialog(Dialog* dialog, qcc::String const& indent)
{
    printBasicWidget(dialog, "Dialog", indent);
    std::cout << indent.c_str() << "Dialog message: " << dialog->getMessage().c_str() << std::endl;
    std::cout << indent.c_str() << "Dialog numActions: " << dialog->getNumActions() << std::endl;
    if (dialog->getLabelAction1().size()) {
        std::cout << indent.c_str() << "Dialog Label for Action1: " << dialog->getLabelAction1().c_str() << std::endl;
    }
    if (dialog->getLabelAction2().size()) {
        std::cout << indent.c_str() << "Dialog Label for Action2: " << dialog->getLabelAction2().c_str() << std::endl;
    }
    if (dialog->getLabelAction3().size()) {
        std::cout << indent.c_str() << "Dialog Label for Action3: " << dialog->getLabelAction3().c_str() << std::endl;
    }
}

void ControllerUtil::printBasicWidget(Widget* widget, qcc::String const& widgetType, qcc::String const& indent)
{
    std::cout << indent.c_str() << widgetType.c_str() << " name: " << widget->getWidgetName().c_str() << std::endl;
    std::cout << indent.c_str() << widgetType.c_str() << " version: " << widget->getInterfaceVersion() << std::endl;
    std::cout << indent.c_str() << widgetType.c_str() << " is " << (widget->getIsSecured() ? "secured" : "not secured") << std::endl;
    std::cout << indent.c_str() << widgetType.c_str() << " is " << (widget->getIsEnabled() ? "enabled" : "not enabled") << std::endl;
    if (widget->getLabel().size()) {
        std::cout << indent.c_str() << widgetType.c_str() << " label: " << widget->getLabel().c_str() << std::endl;
    }
    if (widget->getBgColor() != UINT32_MAX) {
        std::cout << indent.c_str() << widgetType.c_str() << " bgColor: " << widget->getBgColor() << std::endl;
    }
    printHints(widget, widgetType, indent);
}

void ControllerUtil::printErrorWidget(Widget* widget, qcc::String const& indent)
{
    std::cout << indent.c_str() << "Received error widget with name: " << widget->getWidgetName().c_str() << std::endl;
    if (widget->getLabel().size()) {
        std::cout << indent.c_str() << "ErrorWidget label: " << widget->getLabel().c_str() << std::endl;
    }
}

void ControllerUtil::printConstraintList(const std::vector<ConstraintList>& constraintList, PropertyType propertyType, qcc::String const& indent)
{
    for (size_t i = 0; i < constraintList.size(); i++) {
        std::cout << indent.c_str() << "ConstraintList " << i << " Display: " << constraintList[i].getDisplay().c_str() << std::endl;
        switch (propertyType) {
        case UINT16_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().uint16Value << std::endl;
            break;

        case INT16_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().int16Value << std::endl;
            break;

        case UINT32_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().uint32Value << std::endl;
            break;

        case INT32_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().int32Value << std::endl;
            break;

        case UINT64_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().uint64Value << std::endl;
            break;

        case INT64_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().int64Value << std::endl;
            break;

        case DOUBLE_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().doubleValue << std::endl;
            break;

        case STRING_PROPERTY:
            std::cout << indent.c_str() << "ConstraintList " << i << " Value: " << constraintList[i].getConstraintValue().charValue << std::endl;
            break;

        default:
            std::cout << indent.c_str() << "ConstraintList is unknown property type" << std::endl;
            break;
        }
    }
}

void ControllerUtil::ListValue(Property* property, PROPERTY_MULTIMAP &MySTL)
{
		PropertyObject temp_object;
		uint32_t index = MySTL.size(); 	  
 	  temp_object.index = index;
	  temp_object.value = StringPropertyValue(property->getPropertyValue(), property->getPropertyType());
		temp_object.label = property->getLabel().c_str();
		temp_object.hints = property->getHints();
		temp_object.propertyType = property->getPropertyType();
		temp_object.is_writable = property->getIsWritable();
		temp_object.has_range = property->getConstraintRange() ? true: false;
		temp_object.has_list = property->getConstraintList().size() ? true : false;

		if (temp_object.has_range == true){
			temp_object.min_value = property->getConstraintRange()->getMinValue().uint16Value;
			temp_object.max_value = property->getConstraintRange()->getMaxValue().uint16Value;
		}	
		if (temp_object.has_list == true){
			temp_object.constraintList = property->getConstraintList();
			//printConstraintList(temp_object.constraintList, temp_object.propertyType, "  ");
		}		
		MySTL.insert(std::pair<qcc::String, PropertyObject>(property->getWidgetName().c_str(), temp_object));    	
}

void ControllerUtil::printConstraintRange(ConstraintRange* constraintRange, PropertyType propertyType, qcc::String const& indent)
{
    switch (propertyType) {
    case UINT16_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().uint16Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().uint16Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().uint16Value << std::endl;
        break;

    case INT16_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().int16Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().int16Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().int16Value << std::endl;
        break;

    case UINT32_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().uint32Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().uint32Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().uint32Value << std::endl;
        break;

    case INT32_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().int32Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().int32Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().int32Value << std::endl;
        break;

    case UINT64_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().uint64Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().uint64Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().uint64Value << std::endl;
        break;

    case INT64_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().int64Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().int64Value << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().int64Value << std::endl;
        break;

    case DOUBLE_PROPERTY:
        std::cout << indent.c_str() << "ConstraintRange MinValue: " << constraintRange->getMinValue().doubleValue << std::endl;
        std::cout << indent.c_str() << "ConstraintRange MaxValue: " << constraintRange->getMaxValue().doubleValue << std::endl;
        std::cout << indent.c_str() << "ConstraintRange IncrementValue: " << constraintRange->getIncrementValue().doubleValue << std::endl;
        break;

    default:
        std::cout << indent.c_str() << "ConstraintRange is unknown property type" << std::endl;
        break;
    }
}

void ControllerUtil::printPropertyValue(PropertyValue propertyValue, PropertyType propertyType, qcc::String const& indent)
{
    switch (propertyType) {
    case UINT16_PROPERTY:
        std::cout << indent.c_str() << "Property is a UINT16 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.uint16Value << std::endl;
        break;

    case INT16_PROPERTY:
        std::cout << indent.c_str() << "Property is a INT16 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.int16Value << std::endl;
        break;

    case UINT32_PROPERTY:
        std::cout << indent.c_str() << "Property is a UINT32 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.uint32Value << std::endl;
        break;

    case INT32_PROPERTY:
        std::cout << indent.c_str() << "Property is a INT32 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.int32Value << std::endl;
        break;

    case UINT64_PROPERTY:
        std::cout << indent.c_str() << "Property is a UINT64 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.uint64Value << std::endl;
        break;

    case INT64_PROPERTY:
        std::cout << indent.c_str() << "Property is a INT64 Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.int64Value << std::endl;
        break;

    case DOUBLE_PROPERTY:
        std::cout << indent.c_str() << "Property is a DOUBLE Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.doubleValue << std::endl;
        break;

    case STRING_PROPERTY:
        std::cout << indent.c_str() << "Property is a STRING Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.charValue << std::endl;
        break;

    case BOOL_PROPERTY:
        std::cout << indent.c_str() << "Property is a BOOL Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << (propertyValue.boolValue ? "true" : "false") << std::endl;
        break;

    case DATE_PROPERTY:
        std::cout << indent.c_str() << "Property is a Date Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.dateValue->getDay() << "/" <<
        propertyValue.dateValue->getMonth() << "/" <<
        propertyValue.dateValue->getYear() << std::endl;
        break;

    case TIME_PROPERTY:
        std::cout << indent.c_str() << "Property is a Time Property." << std::endl;
        std::cout << indent.c_str() << "Property Value: " << propertyValue.timeValue->getHour() << ":" <<
        propertyValue.timeValue->getMinute() << ":" <<
        propertyValue.timeValue->getSecond() << std::endl;
        break;

    default:
        std::cout << indent.c_str() << "Property is unknown property type" << std::endl;
        break;
    }
}

void ControllerUtil::executeActions(std::vector<Action*>& actionsToExecute)
{
    for (size_t i = 0; i < actionsToExecute.size(); i++) {
        std::cout << "Execute Action for " << actionsToExecute[i]->getWidgetName().c_str() << std::endl;
        QStatus status = actionsToExecute[i]->executeAction();
        std::cout << "    Action execution " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
    }
}

void ControllerUtil::executeDialogActions(std::vector<Dialog*>& dialogsToExecute)
{
    for (size_t i = 0; i < dialogsToExecute.size(); i++) {
        std::cout << "Execute Actions for " << dialogsToExecute[i]->getWidgetName().c_str() << " which has " <<
        dialogsToExecute[i]->getNumActions() << " actions: " << std::endl;
        QStatus status = dialogsToExecute[i]->executeAction1();
        std::cout << "    Action 1 execution " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
        status = dialogsToExecute[i]->executeAction2();
        std::cout << "    Action 2 execution " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
        status = dialogsToExecute[i]->executeAction3();
        std::cout << "    Action 3 execution " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
    }
}

void ControllerUtil::setPropertyValues(std::vector<Property*>& propertiesToChange)
{
    for (size_t i = 0; i < propertiesToChange.size(); i++) {
        std::cout << "Setting Value for Property " << propertiesToChange[i]->getWidgetName().c_str() << std::endl;
        QStatus status = ER_OK;
        switch (propertiesToChange[i]->getPropertyType()) {
        case UINT16_PROPERTY:
            std::cout << "    Setting Value to " << 130 << std::endl;
            status = propertiesToChange[i]->setValue((uint16_t)130);
            break;

        case INT16_PROPERTY:
            std::cout << "    Setting Value to " << 140 << std::endl;
            status = propertiesToChange[i]->setValue((int16_t)140);
            break;

        case UINT32_PROPERTY:
            std::cout << "    Setting Value to " << 150 << std::endl;
            status = propertiesToChange[i]->setValue((uint32_t)150);
            break;

        case INT32_PROPERTY:
            std::cout << "    Setting Value to " << 160 << std::endl;
            status = propertiesToChange[i]->setValue((int32_t)160);
            break;

        case UINT64_PROPERTY:
            std::cout << "    Setting Value to " << 170 << std::endl;
            status = propertiesToChange[i]->setValue((uint64_t)170);
            break;

        case INT64_PROPERTY:
            std::cout << "    Setting Value to " << 180 << std::endl;
            status = propertiesToChange[i]->setValue((int64_t)180);
            break;

        case DOUBLE_PROPERTY:
            std::cout << "    Setting Value to " << 190 << std::endl;
            status = propertiesToChange[i]->setValue((double)190);
            break;

        case STRING_PROPERTY:
            std::cout << "    Setting Value to 'TestingValue'" << std::endl;
            status = propertiesToChange[i]->setValue("TestingValue");
            break;

        case BOOL_PROPERTY:
            std::cout << "    Setting Value to true" << std::endl;
            status = propertiesToChange[i]->setValue(true);
            break;

        case DATE_PROPERTY: {
                std::cout << "    Setting Value to 22/6/1892" << std::endl;
                CPSDate date(22, 6, 1892);
                status = propertiesToChange[i]->setValue(date);
                break;
            }

        case TIME_PROPERTY: {
                std::cout << "    Setting Value to 12:10:30" << std::endl;
                CPSTime time(12, 10, 30);
                status = propertiesToChange[i]->setValue(time);
                break;
            }

        case UNDEFINED: {
                status = ER_NOT_IMPLEMENTED;
                break;
            }
        }
        std::cout << "    Set Value " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
    }
}

void ControllerUtil::printHints(ajn::services::Widget* widget, qcc::String const& widgetType, qcc::String const& indent)
{
    std::vector<uint16_t> hints = widget->getHints();
    if (hints.size()) {
        std::cout << indent.c_str() << widgetType.c_str() << " hints: ";
        for (size_t i = 0; i < hints.size(); i++) {
            switch (widget->getWidgetType()) {
            case WIDGET_TYPE_ACTION:
            case WIDGET_TYPE_ACTION_WITH_DIALOG:
                if (hints[i] == ACTIONBUTTON) {
                    std::cout << "ACTIONBUTTON";
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_LABEL:
                if (hints[i] == TEXTLABEL) {
                    std::cout << "TEXTLABEL";
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_PROPERTY:
                if (hints[i] > 0 && hints[i] < PROPERTY_HINTS_SIZE) {
                    std::cout << PROPERTY_HINTS_STRINGS[hints[i]].c_str() << "," << hints[i];
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_CONTAINER:
                if (hints[i] == VERTICAL_LINEAR) {
                    std::cout << "VERTICAL_LINEAR";
                } else if (hints[i] == HORIZONTAL_LINEAR) {
                    std::cout << "HORIZONTAL_LINEAR";
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_DIALOG:
                if (hints[i] == ALERTDIALOG) {
                    std::cout << "ALERTDIALOG";
                } else {
                    std::cout << "UNKNOWN";
                }
                break;

            case WIDGET_TYPE_ERROR:
                std::cout << "UNKNOWN";
                break;
            }
            std::cout << (i == hints.size() - 1 ? "\n" : ", ");
        }
    }
}
