////////////////////////////////////////////////////////////////////////////////
//  Contact.h
//  Furiosity
//
//  Created by Bojan Endrovski on 23/07/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef CONTACT_H
#define CONTACT_H

#include "Frmath.h"
#include "Entity2D.h"
#include "DynamicEntity2D.h"
#include "Entity3D.h"

namespace Furiosity
{
    ///
    /// A struct that encapsulates the contact between two rigid bodies.
    // Templated to handle both, 2D and 3D cases
    ///
    template<class EntityT, class VectorT>
    class ContactBase
    {
    public:
        /// Holds the bodies that are involved in the contact. The
        /// second of these can be null, for contacts with the scenery.
        EntityT* FirstBody;
        //
        EntityT* SecondBody;
        
        /// Normal restitution coefficient at the contact.
        float Restitution;
        
        /// Direction of the contact in world coordinates.
        VectorT ContactNormal;
        
        // Depth of penetration at the contact.
        float Penetration;
        
        /// A mechanism to detect collisions to raise event but
        /// not reslove in the physics engine
        bool Resolved;
        
        bool VelocityResloved;
        
        /// Default init ctor
        ContactBase() : FirstBody(NULL),
                        SecondBody(NULL),
                        Restitution(0.98f),
                        ContactNormal(VectorT()),
                        Penetration(0.0f),
                        Resolved(false),
                        VelocityResloved(false) {}
        
        /// Creates a new contact among two dynamic bodies
        ContactBase(EntityT* firstBody,
                    EntityT* secondBody,
                    float restitution,
                    VectorT contactNormal,
                    float penetration)
        :   FirstBody(firstBody),
            SecondBody(secondBody),
            Restitution(restitution),
            ContactNormal(contactNormal),
            Penetration(penetration),
            Resolved(false),
            VelocityResloved(false) {}
        
        inline int Type()
        {
            return SecondBody ? FirstBody->EntityType() | SecondBody->EntityType() : FirstBody->EntityType();
        }
    };
    
    /// A 2D contact
    class Contact : public ContactBase<Entity2D, Vector2>
    {
    public:

         inline float SeparatingVelocity()
         {
             Vector2 relativeVelocity(0, 0);
             if(FirstBody && !FirstBody->HasInifitesMass())
             {
                 DynamicEntity2D* mv = static_cast<DynamicEntity2D*>(FirstBody);
                 relativeVelocity = mv->Velocity();
             }
             
             if(SecondBody && !SecondBody->HasInifitesMass())
             {
                 DynamicEntity2D* mv = static_cast<DynamicEntity2D*>(SecondBody);
                 relativeVelocity -= mv->Velocity();
             }
             return relativeVelocity.DotProduct(ContactNormal);
         }
    };
    
    /// A 3D contact
    typedef ContactBase<Entity3D, Vector3> Contact3D;
    
    
}

#endif