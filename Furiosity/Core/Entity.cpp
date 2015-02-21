////////////////////////////////////////////////////////////////////////////////
//  Entity.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/13/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Entity.h"

using namespace Furiosity;
using namespace std;
using namespace tinyxml2;

int Entity::nextValidID = 1;

////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
Entity::Entity() :type(default_entity_type), tag(false)
{
    SetID(nextValidID);
}

////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
Entity::Entity(uint ID)
:	type(default_entity_type),
    tag(false)
{
    SetID(ID);
}


////////////////////////////////////////////////////////////////////////////////
// Ctor from xml stettings, this need to point inside the element and
// not on it's root. This alows for the ctor to be used for settings and
// streaming at the same time.
////////////////////////////////////////////////////////////////////////////////
Entity::Entity(const XMLElement* settings)
:   type(default_entity_type),
    tag(false)
{
    //                      ID
    // Handle id - used for streaming a level with hardcoded ID
    const char* pID = settings->Attribute("ID");
    if (pID)
        SetID(atoi(pID));
    else
        SetID(nextValidID);
    
    //                      Name
    // Set the name for this entity
    //string tName(settings->Attribute("name"));
    const char* pName = settings->Attribute("name");
    if (pName)
        name = string(pName);
    else
        name = "Unknown";
    
    //                      Type
    // Set type - A hacky short cut!
    // !!! Do this only if you know what are you doing as it does no checks against
    // the world types which are usefull for collision and casting !!!
    const char* pType = settings->Attribute("type");
    if(pType)
        type = atoi(pType);
}


////////////////////////////////////////////////////////////////////////////////
// SetID
// This must be called within each constructor to make sure the ID is set
// correctly. It verifies that the value passed to the method is greater
// or equal to the next valid ID, before setting the ID and incrementing
// the next valid ID
////////////////////////////////////////////////////////////////////////////////
void Entity::SetID(uint val)
{
	// Make sure the val is equal to or greater than the next available ID
	assert( (val >= nextValidID) );
	
	_ID = val;
	
	nextValidID = _ID + 1;
}