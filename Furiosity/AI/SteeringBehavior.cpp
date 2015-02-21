////////////////////////////////////////////////////////////////////////////////
//  SteeringBehavior.cpp
//
//  Created by Bojan Endrovski on 20/03/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

// Framework includes
//

// Local includes
#include "SteeringBehavior.h"
#include "Furiosity.h"
#include "DebugDraw2D.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Ctor
// Creates a new behavior for an agent
////////////////////////////////////////////////////////////////////////////////
SteeringBehavior::SteeringBehavior(Vehicle2D* vehicle) :
    vehicle(vehicle),
    flags(none),
    deceleration(1.0f),
    summingMethod(Prioritized),
    waypointDist(0.8f),
    targetAgent1(0),
    targetAgent2(0)
{}

////////////////////////////////////////////////////////////////////////////////
// Dtor - does nothing
////////////////////////////////////////////////////////////////////////////////
SteeringBehavior::~SteeringBehavior() {}

////////////////////////////////////////////////////////////////////////////////
// Calculate
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::Calculate()
{
    switch (summingMethod)
    {
        case WeightedAverage:
            return CalculateWeightedSum();
            
        case Prioritized:
            return CalculatePrioritized();
            
        default:
            return Vector2();
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//                           - SUMMING METHODS -
//
////////////////////////////////////////////////////////////////////////////////

bool SteeringBehavior::AccumulateForce(Vector2 &runningTotal, Vector2 forceToAdd)
{
    // Calculate how much steering force the vehicle has used so far
    float magnitudeSoFar = runningTotal.Magnitude();
    
    // Calculate how much steering force remains to be used by this vehicle
    float magnitudeRemaining = vehicle->MaxForce() - magnitudeSoFar;
    
    // Return false if there is no more force left to use
    if (magnitudeRemaining <= 0.0) return false;
    
    // Calculate the magnitude of the force we want to add
    float magnitudeToAdd = forceToAdd.Magnitude();
    
    // If the magnitude of the sum of forceToAdd and the running total
    // does not exceed the maximum force available to this vehicle, just
    // add together. Otherwise add as much of the forceToAdd vector is
    // possible without going over the max.
    if (magnitudeToAdd < magnitudeRemaining)
    {
        runningTotal += forceToAdd;
        return true;
    }
    else
    {
        forceToAdd.Normalize();
        // Add it to the steering force
        runningTotal += (forceToAdd * magnitudeRemaining);
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Caclulate weighted sum
// This simply sums up all the active behaviors X their weights and 
// truncates the result to the max available steering force before 
// returning.
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::CalculateWeightedSum()
{
    // Set to [0,0]
    steeringForce.Clear();
    
    if(On(seek))
    {
        steeringForce += Seek(target);
    }
    
    if(On(arrive))
    {
        steeringForce += Arrive(target);
    }
    
    if(On(pursuit))
    {
        steeringForce += Pursuit(targetAgent1);
    }
    
    if(On(evade))
    {
        steeringForce += Evade(targetAgent1);
    }
    
    if(On(offset_pursuit))
    {
        steeringForce += OffsetPursuit(targetAgent1, offset);
    }
    
    return steeringForce;
}

////////////////////////////////////////////////////////////////////////////////
// CalculatePrioritized
// This method calls each active steering behavior in order of priority
// and acumulates their forces until the max steering force magnitude
// is reached, at which time the function returns the steering force 
// accumulated to that  point.
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::CalculatePrioritized()
{
    // Set to [0,0]
    steeringForce.Clear();
    
    Vector2 force;
    
    if (On(obstacle_avoidance))
    {
        force = ObstacleAvoidance( vehicle->World()->Entites() ) * 3.0f;
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    if(On(evade))
    {
        force = Evade(targetAgent1);
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }

    
    if (On(seek))
    {
        force = Seek(target);
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    if(On(arrive))
    {
        force += Arrive(target);
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    if(On(pursuit))
    {
        force = Pursuit(targetAgent1);
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    if(On(offset_pursuit))
    {
        force = OffsetPursuit(targetAgent1, offset);
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    if(On(follow_path))
    {
        force = FollowPath();
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    {
        force = Striaght() * 0.0f;
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    return steeringForce;
}

////////////////////////////////////////////////////////////////////////////////
//
//                         - SUMMING METHODS END -
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//                             - BEHAVIORS -
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Seek
// Given a target, this behavior returns a steering force which will
// direct the agent towards the target
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::Seek(const Vector2 &targetPos)
{
#ifdef DEBUG
    gDebugDraw2D.AddCircle(targetPos, waypointDist, Color::Green);
#endif
    
    // Get desired velocity
    Vector2 desiredVelocity = targetPos - vehicle->Position();
    desiredVelocity.Normalize();
    desiredVelocity *= vehicle->MaxSpeed();
    
    // Return the difference
    return desiredVelocity - vehicle->Velocity();    
}

////////////////////////////////////////////////////////////////////////////////
// Flee
// Does the opposite of Seek
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::Flee(const Vector2 &targetPos)
{
    //only flee if the target is within 'panic distance'. Work in distance
    //squared space.
    /*const double PanicDistanceSq = 100.0f * 100.0;
     if (Vec2DDistanceSq(m_pVehicle->Pos(), target) > PanicDistanceSq)
     {
     return Vector2D(0,0);
     }
     */

    // Get desired velocity
    Vector2 desiredVelocity = vehicle->Position() - targetPos;
    desiredVelocity.Normalize();
    desiredVelocity *= vehicle->MaxSpeed();
    //
    return (desiredVelocity - vehicle->Velocity());
}

////////////////////////////////////////////////////////////////////////////////
// Arrive
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::Arrive(const Furiosity::Vector2 &targetPos)
{
#ifdef DEBUG
    //gDebugDraw2D.AddCircle(targetPos, 10.0f, Color::Green);
#endif
    // Vector to target
    Vector2 toTarget = targetPos - vehicle->Position();
    
    // Distance to it
    float distance = toTarget.Magnitude();
    
    if(distance > 1E-10)
    {
        // calculate the speed required to reach the target given the desired
        // deceleration
        float speed =  distance / deceleration;
        
        // make sure the velocity does not exceed the max
        speed = min(speed, vehicle->MaxSpeed());
        
        // from here proceed just like Seek except we don't need to normalize 
        // the ToTarget vector because we have already gone to the trouble
        // of calculating its length: dist. 
        Vector2 desiredVelocity =  toTarget * (speed / distance);
        
        // All done
        return (desiredVelocity - vehicle->Velocity());
    }
    
    return Vector2(0.0f, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////
// Pursuit
// This behavior creates a force that steers the agent towards the  evader
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::Pursuit(const DynamicEntity2D* agent)
{
    // If the evader is ahead and facing the agent then we can just seek
    // for the evader's current position.
    Vector2 toEvader = agent->Position() - vehicle->Position();
    
    float relativeHeading = vehicle->Heading().DotProduct(agent->Heading());
    
    if (    (toEvader.DotProduct(vehicle->Heading()) > 0) &&  
            (relativeHeading < -0.95))          // acos(0.95)=18 degs
    {
        return Seek(agent->Position());
    }
    
    // Not considered ahead so we predict where the evader will be.
    
    // The lookahead time is propotional to the distance between the evader
    // and the pursuer; and is inversely proportional to the sum of the
    // agent's velocities
    float LookAheadTime =  toEvader.Magnitude() /
                            (vehicle->MaxSpeed() + agent->Speed());
    
    //now seek to the predicted future position of the evader
    return Seek(agent->Position() + (agent->Velocity() * LookAheadTime));
}

////////////////////////////////////////////////////////////////////////////////
// Evade
// Similar to pursuit except the agent Flees from the estimated future
// position of the pursuer
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::Evade(const DynamicEntity2D*pursuer)
{
    // Not necessary to include the check for facing direction this time
    
    Vector2 toPursuer = pursuer->Position() - vehicle->Position();
    
    // Uncomment the following two lines to have Evade only consider pursuers 
    // within a 'threat range'
    const float threatRange = 100.0f;
    //
    if (toPursuer.SquareMagnitude() > threatRange * threatRange)
        return Vector2();
    
    // The lookahead time is propotional to the distance between the pursuer
    // and the pursuer; and is inversely proportional to the sum of the
    // agents' velocities
    float lookAheadTime =   toPursuer.Magnitude() / 
                            (vehicle->MaxSpeed() + pursuer->Speed());
    
    // Now flee away from predicted future position of the pursuer
    return Flee(pursuer->Position() + pursuer->Velocity() * lookAheadTime);

}

////////////////////////////////////////////////////////////////////////////////
// OffsetPursuit
// This behavior creates a force that steers the agent towards a formation lead
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::OffsetPursuit(const DynamicEntity2D* leader, Vector2 offset)
{
    // Get vehicle coordinate frame
    Matrix33 transform;
    transform.SetTransform(leader->Heading(), 
                           leader->Side(),
                           leader->Position());
    
    // Transform to world coordinates
    transform.TransformVector2(offset);
    
    //if the evader is ahead and facing the agent then we can just seek
    //for the evader's current position.
    Vector2 toTarget = offset - vehicle->Position();
    
    
    //the lookahead time is propotional to the distance between the evader
    //and the pursuer; and is inversely proportional to the sum of the
    //agent's velocities
    float LookAheadTime =   toTarget.Magnitude() /
                            (vehicle->MaxSpeed() + leader->Speed());
    
    //now seek to the predicted future position of the evader
    return Seek(offset + (leader->Velocity() * LookAheadTime));
}

////////////////////////////////////////////////////////////////////////////////
// FollowPath
// Given path as a series of Vector2Ds, this method produces a force that
// will move the agent along the waypoints in order. The agent uses the
// 'Seek' behavior to move to the next waypoint - unless it is the last
//  waypoint, in which case it 'Arrives'
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::FollowPath()
{
    if (path.size() == 0)
        return Vector2();
    
    Vector2& waypoint = path.front();
    
    // Check how far it's to there
    Vector2 toWaypoint = waypoint - vehicle->Position();
        
    if(path.size() > 1)
    {
        // Advance to next point if close enough
        if(toWaypoint.Magnitude() < waypointDist)
        {
            path.pop_front();
            waypoint = path.front();
        }
        return Seek(waypoint);
    }
    else
    {
        // Advance to next point if close enough
        if(toWaypoint.Magnitude() < (waypointDist * 0.5f))
        {
            path.pop_front();
            return Vector2();
        }
        return Arrive(waypoint);
    }
}

////////////////////////////////////////////////////////////////////////////////
// ObstacleAvoidance
// Given a vector of CObstacles, this method returns a steering force
// that will prevent the agent colliding with the closest obstacle
////////////////////////////////////////////////////////////////////////////////
Vector2 SteeringBehavior::ObstacleAvoidance(const list<Furiosity::Entity2D*> &entites)
{
    float MinDetectionBoxLength = 120.0f;
    
    // The detection box length is proportional to the agent's velocity
    float boxLength =   MinDetectionBoxLength +
                        (vehicle->Speed() / vehicle->MaxSpeed()) *
                        MinDetectionBoxLength;
    
    // Tag all obstacles within range of the box for processing
    vehicle->World()->TagEntitiesWithinRange(vehicle, boxLength);
    
    // This will keep track of the closest intersecting obstacle (CIB)
    Entity2D* closestIntersectingObstacle = NULL;
    
    // This will be used to track the distance to the CIB
    float distToClosestIP = MAXFLOAT;
    
    // This will record the transformed local coordinates of the CIB
    Vector2 localPosOfClosestObstacle;
    
    // Get local vehicle space and inverse
    Matrix33 toWorld;
    toWorld.SetTransform(vehicle->Heading(),
                         vehicle->Side(),
                         vehicle->Position());
    //
    Matrix33 toLocal = toWorld.Inverse();
    
    std::list<Entity2D*>::const_iterator curOb = entites.begin();
    //
    while(curOb != entites.end())
    {
        Entity2D* bge = (*curOb);
        // If the obstacle has been tagged within range proceed
        if (bge->IsTagged())
        {
            
            // Calculate this obstacle's position in local space                        
            Vector2 localPos = bge->Position();
            toLocal.TransformVector2(localPos);
            
            // If the local position has a negative x value then it must lay
            // behind the agent. (in which case it can be ignored)
            if (localPos.x >= 0)
            {
                // If the distance from the x axis to the object's position is less
                // than its radius + half the width of the detection box then there
                // is a potential intersection.
                float expandedRadius = bge->BoundingRadius() + vehicle->BoundingRadius();
                
                if (Absf(localPos.y) < expandedRadius)
                {
                    // Now to do a line/circle intersection test. The center of the 
                    // circle is represented by (cX, cY). The intersection points are 
                    // given by the formula x = cX +/-sqrt(r^2-cY^2) for y=0. 
                    // We only need to look at the smallest positive value of x because
                    // that will be the closest point of intersection.
                    float cX = localPos.x;
                    float cY = localPos.y;
                    
                    //we only need to calculate the sqrt part of the above equation once
                    float sqrtPart = sqrt(expandedRadius * expandedRadius - cY*cY);
                    
                    float ip = cX - sqrtPart;
                    
                    if (ip <= 0.0)
                    {
                        ip = cX + sqrtPart;
                    }
                    
                    //test to see if this is the closest so far. If it is keep a
                    //record of the obstacle and its local coordinates
                    if (ip < distToClosestIP)
                    {
                        distToClosestIP = ip;
                        
                        closestIntersectingObstacle = bge;
                        
                        localPosOfClosestObstacle = localPos;
                    }         
                }
            }
        }
        
        ++curOb;  
    } // while
    
    // If we have found an intersecting obstacle, calculate a steering 
    // force away from it
    Vector2 steeringForce;
    
    if (closestIntersectingObstacle)
    {
        // The closer the agent is to an object, the stronger the 
        // steering force should be
        float multiplier = 1.0f + (boxLength - localPosOfClosestObstacle.x) / boxLength;
        
        // Calculate the lateral force
        steeringForce.y = (closestIntersectingObstacle->BoundingRadius() -
                           localPosOfClosestObstacle.y)  * multiplier;   
        
        // Apply a braking force proportional to the obstacles distance from
        // the vehicle. 
        const float brakingWeight = 0.2f;
        steeringForce.x = (closestIntersectingObstacle->BoundingRadius() - 
                           localPosOfClosestObstacle.x) * brakingWeight;
        
        // Finally, convert the steering vector from local to world space
        toWorld.TransformNormal2(steeringForce);
    }
    
    // Zero in local space should yield zero force too
    return steeringForce;
}

Vector2 SteeringBehavior::Striaght()
{
    return vehicle->Heading() * ( (vehicle->MaxSpeed() - vehicle->Speed()) / vehicle->MaxSpeed() );
}


////////////////////////////////////////////////////////////////////////////////
//
//                           - BEHAVIORS END -
//
////////////////////////////////////////////////////////////////////////////////

