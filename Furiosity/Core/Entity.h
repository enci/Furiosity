////////////////////////////////////////////////////////////////////////////////
//  Entity.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/13/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "tinyxml2.h"
#include "Messaging.h"
#include "Defines.h"


namespace Furiosity
{
    enum { default_entity_type = 0 };
    
    /// A base class for all things that should exist in a game world
    class Entity
    {
    private:
		/// Each entity has a unique ID
		uint		_ID;
		
		/// This is the next valid ID. Each time a Entity2D is instantiated
		/// this value is updated
		static int  nextValidID;
		
		/// This must be called within each constructor to make sure the ID is set
		/// correctly. It verifies that the value passed to the method is greater
		/// or equal to the next valid ID, before setting the ID and incrementing
		/// the next valid ID
		void        SetID(uint val);
        
    protected:
        
        /// This is a generic flag
		bool		tag;
        
        /// Every entity has a type associated with it (health, troll, ammo etc)
        /// In some game we are using this as a bitmap flag so carefull with this
		int			type;
        
        /// A name for the entity, this helps when debugging
        std::string name;
        
        /// Abstract class
        Entity();

        /// Force an ID
        Entity(uint ID);
        
        /// Ctor from xml settings, this needs to point inside the element and
        /// not on it's root. This allows for the ctor to be used for settings and
        /// streaming at the same time.
        Entity(const tinyxml2::XMLElement* settings);
        
    public:
        
        /// Virtual dtor
        virtual ~Entity() {}
        
        /// The most important ability of an entity
        virtual void        Update(float dt)    {};
        
        /// Called when the entity has been added to a container
        virtual void        Added()             {};
        
        /// Use this to grab the next valid ID
		static uint         NextValidID()       { return nextValidID; }
		
		/// This can be used to reset the next ID (beginning of a level or such)
		static void         ResetNextValidID()  { nextValidID = 1; }
        
		/// Get the id of this entity
		uint                GetID() const       { return _ID; }
        
        /// Get the name of this entity
        const std::string&  Name() const        { return name; }
        
        /// Is this entity tagged
		int                 IsTagged() const    { return tag; }
        
        /// Tag this entity
		void                Tag()               { tag = true; }
        
        /// Untag this entity
		void                UnTag()             { tag = false; }
        
        /// Entity type access
		int                 EntityType() const  { return type; }
        
        /// Entity type can be set only once per entity
		void                SetEntityType(int t)
        { if(type == default_entity_type) type = t; }
        
        /// Used for free form async communication
        virtual void        HandleMessage(const Message& message) {}
    };
}