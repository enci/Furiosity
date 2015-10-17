////////////////////////////////////////////////////////////////////////////////
//  BaseGameEnity.h
//
//  Created by Bojan Endrovski on 26/09/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

// Local includes
#include "Frmath.h"
#include "CollisionShapes.h"
#include "Color.h"
#include "tinyxml2.h"
#include "SpriteRender.h"
#include "Transformable.h"
#include "Countable.h"
#include "Messaging.h"
#include "Entity.h"

using std::vector;
using tinyxml2::XMLElement;

namespace Furiosity
{
    ///
    /// Base Game Entity
    /// A base class for all things that should exist in a game world (scene, level)
    ///
	class Entity2D :    public Entity,
                        public Transformable,
                        public Countable<Entity2D>
	{
        
	protected:
		      
		/// Position and orientation in the envirnoment
        Matrix33    transform;
		
        /// Like mass, but then inverse
        float       inverseMass;
        
        /// This shoudl move out of here eventualy
        //        float       renderLayer;
		
		/// Geometry used for collision checking        
        CollisionShape* collisionShape;
		
		/// Create with ID
		Entity2D(uint ID);
		
	public:
        /// Default ctor, not a good idea
		Entity2D();
		
        /// Ctor for a quick body with a disk as collision shape
		Entity2D(const Vector2& pos, float radius);
        
        /// Ctor with an explicit collision shape
        Entity2D(const Vector2& pos, CollisionShape* chape);
        
        /// Ctor from xml settings, this need to point inside the element and
        /// not on it's root. This allows for the ctor to be used for settings and
        /// streaming at the same time.
        Entity2D(const XMLElement* settings);
				
        /// Simple dtor
		virtual			~Entity2D() { delete collisionShape; }
		
        /// Place your logic inside (if any) 
		virtual void	Update(float dt) {}
        
        /// This is up to the 
        virtual void    HandleMessage(const Message& message) {}
		
#ifdef DEBUG        
		virtual void	DebugRender(Color c = Color::Red) const;
#endif
                
        /// One more like it
        virtual void    Render(SpriteRender* render) { }
        
        /// Gets the bounding radius
        float			BoundingRadius() const;
        
        /// Checks if there id a collision shape defined
        bool            HasCollisionShape() const       { return collisionShape == 0; }
        
        /// Gets the collision shape
        CollisionShape* GetCollisionShape() const       { return collisionShape; }

        
        /// Mass access methods
        float           Mass() const                    { return 1 / inverseMass;       }
        void            SetMass(float m)                { inverseMass = 1 / m;          }
        //
        float           InverseMass() const             { return inverseMass;           }
        void            SetInverseMass(float m)         { inverseMass = m;              }
        //
        bool            HasInifitesMass()               { return inverseMass == 0.0f;   }
    };
}