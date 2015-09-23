////////////////////////////////////////////////////////////////////////////////
//  Vehicle3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/17/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Vehicle3D.h"
#include "DebugDraw3D.h"

using namespace Furiosity;


////////////////////////////////////////////////////////////////////////////////
// Ctor
// Creates a new behavior for an agent
////////////////////////////////////////////////////////////////////////////////
Vehicle3D::Vehicle3D(World3D& world, float radius) :
    DynamicEntity3D(&world, nullptr, radius),
    world(world),
    flags(none),
    deceleration(1.0f),
    waypointDist(0.06f),
    headingSmoother(4)
{}

void Vehicle3D::LoadFromXml(const XMLElement &settings)
{
    DynamicEntity3D::LoadFromXml(settings);
    settings.QueryFloatAttribute("deceleration", &deceleration);
}

void Vehicle3D::Update(float dt)
{
    steeringForce = Calculate();
    force += headingSmoother.Update(steeringForce);
    DynamicEntity3D::Update(dt);
    
#ifdef DEBUG    
    Vector3 next = path.front();
    for (const Vector3& v : path)
    {
        Vector3 prev = next;
        next = v;
        gDebugDraw3D.AddLine(prev, next, Color::White);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Calculate
////////////////////////////////////////////////////////////////////////////////
Vector3 Vehicle3D::Calculate()
{
    return CalculatePrioritized();
}

////////////////////////////////////////////////////////////////////////////////
//
//                           - SUMMING METHODS -
//
////////////////////////////////////////////////////////////////////////////////

bool Vehicle3D::AccumulateForce(Vector3 &runningTotal, Vector3 forceToAdd)
{
    // Calculate how much steering force the vehicle has used so far
    float magnitudeSoFar = runningTotal.Magnitude();
    
    // Calculate how much steering force remains to be used by this vehicle
    float magnitudeRemaining = MaxForce() - magnitudeSoFar;
    
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
// CalculatePrioritized
// This method calls each active steering behavior in order of priority
// and acumulates their forces until the max steering force magnitude
// is reached, at which time the function returns the steering force
// accumulated to that  point.
////////////////////////////////////////////////////////////////////////////////
Vector3 Vehicle3D::CalculatePrioritized()
{
    // Set to [0,0]
    steeringForce.Clear();
    
    Vector3 force;
    
    if(On(flee))
    {
        force = Flee(target);
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    if (On(seek))
    {
        force = Seek(target);
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    if(On(arrive))
    {
        force += Arrive(target, deceleration);
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    if(On(follow_path))
    {
        force = FollowPath();
        if(!AccumulateForce(steeringForce, force)) return steeringForce;
    }
    
    return steeringForce;
}

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
Vector3 Vehicle3D::Seek(const Vector3& targetPos)
{
#ifdef DEBUG
    gDebugDraw3D.AddCircle(targetPos, waypointDist, Color::Green);
#endif
    
    // Get desired velocity
    Vector3 desiredVelocity = targetPos - Position();
    desiredVelocity.Normalize();
    desiredVelocity *= MaxSpeed();
    
    // Return the difference
    return desiredVelocity - Velocity();
}

////////////////////////////////////////////////////////////////////////////////
// Flee
// Does the opposite of Seek
////////////////////////////////////////////////////////////////////////////////
Vector3 Vehicle3D::Flee(const Vector3& targetPos)
{
    // Get desired velocity
    Vector3 desiredVelocity = Position() - targetPos;
    desiredVelocity.Normalize();
    desiredVelocity *= MaxSpeed();
    //
    return (desiredVelocity - Velocity());
}

////////////////////////////////////////////////////////////////////////////////
// Arrive
////////////////////////////////////////////////////////////////////////////////
Vector3 Vehicle3D::Arrive(const Furiosity::Vector3& targetPos, float deceleration)
{
#ifdef DEBUG
    gDebugDraw3D.AddCircle(targetPos, 10.0f, Color::Green);
#endif
    
    // Vector to target
    Vector3 toTarget = targetPos - Position();
    
    // Distance to it
    float distance = toTarget.Magnitude();
    
    if(distance > 1E-5)
    {
        // because Deceleration is enumerated as an int, this value is required
        // to provide fine tweaking of the deceleration..
        //        const float DecelerationTweaker = 0.15f;
        
        // calculate the speed required to reach the target given the desired
        // deceleration
        float speed =  distance / deceleration;
        
        // make sure the velocity does not exceed the max
        speed = min(speed, MaxSpeed());
        
        // from here proceed just like Seek except we don't need to normalize
        // the ToTarget vector because we have already gone to the trouble
        // of calculating its length: dist.
        Vector3 desiredVelocity =  toTarget * (speed / distance);
        
        // All done
        return (desiredVelocity - Velocity());
    }
    
    return Vector3();
}

////////////////////////////////////////////////////////////////////////////////
// FollowPath
// Given path as a series of Vector2Ds, this method produces a force that
// will move the agent along the waypoints in order. The agent uses the
// 'Seek' behavior to move to the next waypoint - unless it is the last
//  waypoint, in which case it 'Arrives'
////////////////////////////////////////////////////////////////////////////////
Vector3 Vehicle3D::FollowPath()
{
    if (path.size() == 0)
        return Vector3();
    
    Vector3& waypoint = path.front();
    
    // Check how far it's to there
    Vector3 toWaypoint = waypoint - Position();
    
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
            return Vector3();
        }

        // Just added this line to make things compile on Android (gerjo)
        float normal = 1;

        return Arrive(waypoint, normal);
    }
}
