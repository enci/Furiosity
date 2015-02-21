////////////////////////////////////////////////////////////////////////////////
//  XmlMacros.h
//
//  Created by Bojan Endrovski on 19/06/14.
//  Copyright (c) 2014 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "tinyxml2.h"

/// Loop to iterate over XML elements:
#define FOREACH_XML_ELEMENT(M_element, M_root) for(XMLNode* M_element = M_root->FirstChild(); M_element != 0; M_element = M_element->NextSibling())

/// Loop to iterate over XML elements:
#define FOREACH_XML_ELEMENT_REF(M_element, M_root) for(const XMLElement* M_element = (M_root).FirstChildElement(); M_element != 0; M_element = M_element->NextSiblingElement())


/// Loop to iterate over XML elements:
#define FOREACH_XML_ELEMENT_NAME(M_element, M_root, M_name) \
for(const XMLElement* M_element = (M_root).FirstChildElement(M_name); M_element != 0; M_element = M_element->NextSiblingElement(M_name))

/// Loop to iterate over each file in a XMLnode or XMLelement. This loop
/// automatically does a ToElement cast, and validates whether or not it
/// actually is an element.
#define FOREACH_XML_ATTRIBUTE(M_attribute, M_node) for(const XMLAttribute* M_attribute = (M_node->ToElement() != 0) ? M_node->ToElement()->FirstAttribute():0; M_attribute != 0; M_attribute = M_attribute->Next())
