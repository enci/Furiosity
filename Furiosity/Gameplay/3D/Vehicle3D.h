////////////////////////////////////////////////////////////////////////////////
//  Vehicle3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/17/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Fr
#include "DynamicEntity3D.h"
#include "ValueSmoother.h"

// STL
#include <list>

namespace Furiosity
{
    class World3D;
    
    ///
    /// A vehicle has moving functionality
    ///
    class Vehicle3D : public DynamicEntity3D
    {
    public:
        typedef std::list<Vector3> Path3D;
        
        /*
        // Arrive makes use of these to determine how quickly a vehicle
        // should decelerate to its target
        enum Deceleration
        {
            slow                = 4,
            normal              = 3,
            fast                = 2,
            faster              = 1
        };
        */

        
    protected:
        // Behavior type
        enum behavior_type
        {
            none                = 0x00000,
            seek                = 0x00001,
            flee                = 0x00002,
            arrive              = 0x00004,
            //wander              = 0x00008,
            //obstacle_avoidance  = 0x00010,
            //pursuit             = 0x00020,
            //evade               = 0x00040,
            //offset_pursuit      = 0x00080,
            follow_path         = 0x00100,
            stop                = 0x00200
        };
        
        // The steering force created by the combined effect of all
        // the selected behaviors
        Vector3 steeringForce;
        
        // The current target
        Vector3 target;
        
        Path3D  path;
        
        // When closer than this distance it will move to the next one
        float   waypointDist;
        
        // One int for all the flags
        int     flags;
        
        // Deceleration used for arrive
        float deceleration;
        
        ValueSmoother<Vector3> headingSmoother;
        
        World3D& world;
        
        /// This function tests if a specific bit of m_iFlags is set
        bool      On(behavior_type bt)  { return (flags & bt) == bt; }
        
        // This behavior moves the agent towards a target position
        Vector3 Seek(const Vector3& targetPos);
        
        
        /// This behavior returns a vector that moves the agent away
        /// from a target position
        Vector3 Flee(const Vector3& targetPos);
        
        
        /// This behavior is similar to seek but it attempts to arrive
        /// at the target position with a zero velocity
        Vector3 Arrive(const Vector3& targetPos, float deceleration);
        
        /// Given path as a series of Vector2Ds, this method produces a force that
        /// will move the agent along the waypoints in order. The agent uses the
        /// 'Seek' behavior to move to the next waypoint - unless it is the last
        ///  waypoint, in which case it 'Arrives'
        Vector3 FollowPath();
        
        /// Accumulates force and returns true if there is more force to be added
        /// and false if the maximum has been reached
        bool AccumulateForce(Vector3 &runningTotal, Vector3 forceToAdd);
        
        Vector3 CalculatePrioritized();
        
        /// Calculates and sums the steering forces from any active behaviors
        virtual Vector3 Calculate();
        
    public:
        Vehicle3D(World3D& world, /*, Entity3D& parent, */ float radius = 0.0f);
        
        virtual ~Vehicle3D() {}
        
        virtual void LoadFromXml(const XMLElement& settings);
        
        virtual void Update(float dt) override;
        
        // Set and get target
        void SetTarget(const Vector3& target) { this->target = target; }
        Vector3 Target() const { return target; }
        
        // Set and get first target agent
        void AddPathWaypoint(const Vector3& waypoint) { path.push_back(waypoint); }
        const Path3D& GetPath() const { return path; }
        void SetPath(const Path3D& path) { this->path = path; }
        
        void SeekOn()               { flags |= seek;    }
        void ArriveOn()             { flags |= arrive;  }
        void FollowPathOn()         { flags |= follow_path; }
        
        void SeekOff()              { if(On(seek))              flags ^= seek;          }
        void ArriveOff()            { if(On(arrive))            flags ^= arrive;        }
        void FollowPathOff()        { if(On(follow_path))       flags ^= follow_path;   }
        void ClearBehaviors()       { flags = none; path.clear();                       }
    };
}



// end