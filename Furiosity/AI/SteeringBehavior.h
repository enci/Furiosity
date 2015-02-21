////////////////////////////////////////////////////////////////////////////////
//  SteeringBehavior.h
//
//  Created by Bojan Endrovski on 20/03/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef STEERINBEHAVIOR_H
#define STEERINBEHAVIOR_H

#include "Vector2.h"
#include "Vehicle2D.h"
//#include "Path.h"

// STL
#include <vector>
#include <queue>

using namespace std;

namespace Furiosity
{
    // Forward declaration
    class Vehicle2D;
    
    //typedef queue<Vector2> Path;
    typedef list<Vector2> Path;
    
    /////////////////////////////////////////////////////////////////////////////
    // A class that encapsulates steering behaviors for a Vehicle2D
    /////////////////////////////////////////////////////////////////////////////
    class SteeringBehavior
    {
    private:
        
        // Behavior type
        enum behavior_type
        {
            none                = 0x00000,
            seek                = 0x00001,
            flee                = 0x00002,
            arrive              = 0x00004,
            wander              = 0x00008,
            obstacle_avoidance  = 0x00010,
            pursuit             = 0x00020,
            evade               = 0x00040,
            offset_pursuit      = 0x00080,
            follow_path         = 0x00100,
            stop                = 0x00200
        };
        
        enum SummingMethod
        {
            WeightedAverage,
            Prioritized,
            Dithered
        };

        
    private:
        //  A pointer to the owner of this instance
        Vehicle2D*    vehicle;
        
        // The steering force created by the combined effect of all
        // the selected behaviors
        Vector2     steeringForce;
        
        // These can be used to keep track of friends, pursuers, or prey
        DynamicEntity2D*    targetAgent1;
        DynamicEntity2D*    targetAgent2;
        
        // The current target
        Vector2     target;
        
        // Offset for offset pursuit
        Vector2     offset;
        
        
        // Current waypoint when following a path
       // int         currentWaypoint;
        
        // Current path
        //Path*       path;
        Path        path;
        
        // When closer than this distance it will move to the next one
        float       waypointDist;
        
        
        // Multipliers
        // These can be adjusted to effect strength of the appropriate behavior.
        // Useful to get flocking the way you require for example.
        float weightSeek;
        float weightFlee;
        float weightArrive;
        
        // One int for all the flags
        int flags;
        
        // Deceleration used for arrive
        //Deceleration deceleration;
        
        /// Deceleration used for arrive
        float deceleration;
        
        SummingMethod summingMethod;

        
    private:    
        
        // This function tests if a specific bit of m_iFlags is set
        bool      On(behavior_type bt)  { return (flags & bt) == bt; }
        
        
        /*********************** Behaviors Declarations *************************/
        
        // This behavior moves the agent towards a target position
        Vector2 Seek(const Vector2& targetPos);
        
        
        // This behavior returns a vector that moves the agent away
        // from a target position
        Vector2 Flee(const Vector2& targetPos);
        
        
        // This behavior is similar to seek but it attempts to arrive 
        // at the target position with a zero velocity
        Vector2 Arrive(const Vector2& targetPos);
        
        
        // This behavior predicts where an agent will be in time T and seeks
        // towards that point to intercept it.
        Vector2 Pursuit(const DynamicEntity2D* agent);
        
        
        // This behavior attempts to evade a pursuer
        Vector2 Evade(const DynamicEntity2D* agent);
        
        
        // This behavior maintains a position, in the direction of offset
        // from the target vehicle
        // Offest vector send as value for edit 
        Vector2 OffsetPursuit(const DynamicEntity2D* agent, Vector2 offset);
        
        
        // This returns a steering force which will attempt to keep the agent 
        // away from any obstacles it may encounter
        Vector2 ObstacleAvoidance(const list<Entity2D*>& entites);
        
        // Used to avoid staling - might use a tiny bit of wander
        // but this is cheaper
        Vector2 Striaght();
        
        
        // Given path as a series of Vector2Ds, this method produces a force that
        // will move the agent along the waypoints in order. The agent uses the
        // 'Seek' behavior to move to the next waypoint - unless it is the last
        //  waypoint, in which case it 'Arrives'
        Vector2 FollowPath();
        
        /*********************** Behaviors Declared *****************************/
                
        // Accumulates force and returns true if there is more force to be added
        // and false if the maximum has been reached
        bool AccumulateForce(Vector2 &runningTotal, Vector2 forceToAdd);
        
        // Calculates and sums the steering forces from any active behaviors
        Vector2 CalculateWeightedSum();
        Vector2 CalculatePrioritized();
        Vector2 CalculateDithered();
        
    public:
        
        // Ctor
        SteeringBehavior(Vehicle2D* agent);
        
        virtual ~SteeringBehavior();
        
        
        // Calculates and sums the steering forces from any active behaviors
        virtual Vector2 Calculate();
        
        // Set and get target
        void SetTarget(const Vector2& target) { this->target = target; }        
        Vector2 Target() const { return target; }
        
        // Set and get first target agent
        void SetTargetAgent1(DynamicEntity2D* agent) { this->targetAgent1 = agent; }
        DynamicEntity2D* TargetAgent1() { return targetAgent1; }
        
        // Set and get second target agent
        void SetTargetAgent2(DynamicEntity2D* agent) { this->targetAgent2 = agent; }
        DynamicEntity2D* TargetAgent2() const { return targetAgent2; }
            
        // Set and get path
        /*
        void SetPath(Path* p) { this->path = p; currentWaypoint = 0; }
        const Path* GetPath() const { return path; }
        int CurrentWayPoint() const { return currentWaypoint; }
         */
        void AddPathWaypoint(const Vector2& waypoint) { path.push_back(waypoint); }
        const Path& GetPath() const { return path; }
        
        
        void FleeOn()                           { flags |= flee;    }
        //
        void SeekOn()                           { flags |= seek;    }
        //
        void ArriveOn()                         { flags |= arrive;  }
        //
        void ArriveOn(float decel)              { flags |= arrive;  deceleration = decel; }
        //
        void PursuitOn(DynamicEntity2D*v)         { flags |= pursuit; targetAgent1 = v; }
        //
        void EvadeOn(DynamicEntity2D*v)           { flags |= evade;   targetAgent1 = v; }
        //
        void OffsetPursuitOn(DynamicEntity2D*v, const Vector2 offset)
        { flags |= offset_pursuit; this->offset = offset; targetAgent1 = v; }
        //
        void ObstacleAvoidanceOn()              { flags |= obstacle_avoidance;}
        //
        void FollowPathOn()                     { flags |= follow_path; }
        // void FollowPathSkipWP()
        // { currentWaypoint = (currentWaypoint + 1) % path->WaypointCount(); }
        
//        void StraightOn();
        
        //void WanderOn(){m_iFlags |= wander;}
        //void CohesionOn(){m_iFlags |= cohesion;}
        //void SeparationOn(){m_iFlags |= separation;}
        //void AlignmentOn(){m_iFlags |= allignment;}
        //void WallAvoidanceOn(){m_iFlags |= wall_avoidance;}
        //void InterposeOn(Vehicle2D* v1, Vehicle2D* v2){m_iFlags |= interpose; m_pTargetAgent1 = v1; m_pTargetAgent2 = v2;}
        //void HideOn(Vehicle2D* v){m_iFlags |= hide; m_pTargetAgent1 = v;}
        //void FlockingOn(){CohesionOn(); AlignmentOn(); SeparationOn(); WanderOn();}
        
        void FleeOff()              { if(On(flee))              flags ^= flee;           }
        void SeekOff()              { if(On(seek))              flags ^= seek;           }
        void ArriveOff()            { if(On(arrive))            flags ^= arrive;         }
        void EvadeOff()             { if(On(evade))             flags ^= evade;          }
        void PursuitOff()           { if(On(pursuit))           flags ^= pursuit;             }
        void OffsetPursuitoff()     { if(On(offset_pursuit))    flags ^= offset_pursuit;      }
        void ObstacleAvoidanceOff() { if(On(obstacle_avoidance))flags ^= obstacle_avoidance; }        
        void FollowPathOff()        { if(On(follow_path))       flags ^= follow_path; }        
        void ClearBehaviors()       { flags = none; path.clear(); /* currentWaypoint = 0; */ }
                //void WanderOff(){if(On(wander)) m_iFlags ^=wander;}
        
        void SetDeceleration(float decel) { deceleration = decel; }
        
    };
}

#endif