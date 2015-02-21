////////////////////////////////////////////////////////////////////////////////
//  Messaging.h
//
//  Created by Bojan Endrovski on 6/21/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Furiosity
{
    /// Type of a message. Starting position for user messages is MESSAGE_TYPE_COUNT
    enum MessageType
    {
        ENTITY_DELETED_MSG,
        MESSAGE_TYPE_COUNT
    };
    
    ///
    /// Generic message
    ///
    class Message
    {
    protected:
        
        // Generic type
        int type;
        
    public:
        
        // Creates a new message with a type
        Message(int type) : type(type)  {}
        
        // Destroys this message
        virtual ~Message()              {}
        
        // Get the type
        int     Type() const            { return type; }
    };
    
    
    ///
    /// EntityDeletedMessage, sent when an entity has been destroyed
    ///
    class Entity;
    class EntityDeletedMessage : public Message
    {
        // Reference to the destroyed entity
        const Entity& bge;
        
    public:
        // Create a new EntityDeletedMessage with an entity
        EntityDeletedMessage(const Entity & entity) :
            bge(entity),
            Message(ENTITY_DELETED_MSG)
        {}
        
        // Get the entity
        const Entity& Entity() const { return bge; }
    };

}

