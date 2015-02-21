////////////////////////////////////////////////////////////////////////////////
//  CollisionMethods.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/23/11.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#include "CollisionMethods.h"
#include "Frmath.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//
//                      TOOLS
//
////////////////////////////////////////////////////////////////////////////////

// Creats an interval (swaps left and right if needed)
void CreateInterval(float& left, float& right)
{
    if(left > right)
    {
        float t = left;
        left = right;
        right = t;
    }
}

// Extends and interval if needed
void AddToInterval(float& left, float& right, float value)
{
    if (value < left)
        left = value;
    else if (value > right)
        right = value;
}

void CreateInterval(float& left,
                    float& right,
                    const Vector2& dir, 
                    const Vector2* vertices,
                    int length)
{
    
}

////////////////////////////////////////////////////////////////////////////////
// Check if two itervals ovelap and how much
////////////////////////////////////////////////////////////////////////////////
float IntervalOverlap(float leftA, float rightA, float leftB, float rightB)
{
    if (rightA < leftB || rightB < leftA)
        return 0;
    
    float ovrlp0 = rightA - leftB;
    float ovrlp1 = rightB - leftA;
    
    if (ovrlp0 > ovrlp1)
        return ovrlp1;
    else
        return ovrlp0;
}

////////////////////////////////////////////////////////////////////////////////
// Gets the closest point on a line to a circle
////////////////////////////////////////////////////////////////////////////////
Vector2 LineToDiskClosestPoint(const Vector2& center,
                               const Vector2& A,
                               const Vector2& B)
{
    // Get basic vector
    Vector2 dir     = B - A;        // Along the line seg
    Vector2 localC  = center - A;   // The center in local coords
    
    // Check if the line seg has length
    if(dir.Magnitude() <= 0.0f)
        return A;
    
    // Project the disk
    Vector2 unitDir = dir.Unit();
    float proj      = localC.DotProduct(unitDir);
    
    // Check for the end points
    if(proj < 0)
        return A;
    if(proj > dir.Magnitude())
        return B;
    
    // Return closest
    Vector2 projL = dir.Unit() * proj;  // Get the position in local coords
    Vector2 projW = A + projL;          // Move to world  coords
    return projW;
}


////////////////////////////////////////////////////////////////////////////////
//
//                              TOOLS END
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Any collision shape to a line segment (read walls)
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::CollisionShapeToLineSeg(CollisionShape const* shape,
                                        LineSegment const* line,
                                        Contact* contact)
{
    switch (shape->ShapeEnum)
    {            
        case COLLISION_SHAPE_DISK:
        {
            const Disk& disk = *static_cast<Disk const*>(shape);
            return DiskToLineSeg(disk, *line, Matrix33::Identity, *contact);
        }    
        default:
            return false;
    }
}
                     
////////////////////////////////////////////////////////////////////////////////
// Shape to shape
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::ShapeToShape(CollisionShape const* shapeOne,
                             CollisionShape const* shapeTwo,
                             Contact* contact)
{
    contact->Resolved = false;
    contact->VelocityResloved = false;
    
    switch (shapeOne->ShapeEnum)
    {
        //////////////////////////////////////////////////////////////////////
        // Disk to stuff
        //////////////////////////////////////////////////////////////////////
        case COLLISION_SHAPE_DISK:
        {
            switch (shapeTwo->ShapeEnum)
            {
                case COLLISION_SHAPE_DISK:
                {
                    // Disk to disk
                    const Disk& diskOne        = *static_cast<Disk const*>(shapeOne);
                    const Disk& diskTwo        = *static_cast<Disk const*>(shapeTwo);
                    return DiskToDisk(diskOne, diskTwo, *contact);                   
                }
                    
                case COLLISION_SHAPE_BOX:
                {
                    // Disk to box
                    const Disk& disk    = *static_cast<Disk const*>(shapeOne);
                    const Box& box      = *static_cast<Box const*>(shapeTwo);
                    if(BoxToDisk(box, disk, *contact))
                    {
                        contact->ContactNormal *= -1.0f;
                        return true;
                    }
                    break;
                }
                
                case COLLISION_SHAPE_POLYLINE:
                {
                    // Disk to Line
                    const Disk& disk        = *static_cast<Disk const*>(shapeOne);
                    const Polyline& line    = *static_cast<Polyline const*>(shapeTwo);
                    return DiskToPolyline(disk, line, *contact);                   
                }
                    
                default:
                    return false;
            }            
        }
        
        //////////////////////////////////////////////////////////////////////
        // Box with stuff
        //////////////////////////////////////////////////////////////////////
        case COLLISION_SHAPE_BOX:
        {
            switch (shapeTwo->ShapeEnum)
            {
                case COLLISION_SHAPE_DISK:
                {
                    // Box to disk
                    const Box& box      = *static_cast<Box const*>(shapeOne);
                    const Disk& disk    = *static_cast<Disk const*>(shapeTwo);
                    return BoxToDisk(box, disk, *contact);                   
                }
                    
                case COLLISION_SHAPE_BOX:
                {
                    // box to box
                    const Box& boxOne   = *static_cast<Box const*>(shapeOne);
                    const Box& boxTwo   = *static_cast<Box const*>(shapeTwo);
                    return BoxToBox(boxOne, boxTwo, *contact);
                }
                    
                case COLLISION_SHAPE_POLYLINE:
                    // box to line
                    break;
                    
                default:
                    return false;
            }
        }
            
        //////////////////////////////////////////////////////////////////////
        // Polyline to stuff
        //////////////////////////////////////////////////////////////////////
        case COLLISION_SHAPE_POLYLINE:
        {
            switch (shapeTwo->ShapeEnum)
            {
                case COLLISION_SHAPE_DISK:
                {
                    // Disk to Line
                    const Disk& disk        = *static_cast<Disk const*>(shapeTwo);
                    const Polyline& line    = *static_cast<Polyline const*>(shapeOne);
                    if( DiskToPolyline(disk, line, *contact) )
                    {
                        contact->ContactNormal *= -1.0f;
                        return true;
                    }
                    break;
                }
                    
                case COLLISION_SHAPE_BOX:
                    // polyline to box
                    break;
                    
                case COLLISION_SHAPE_POLYLINE:
                    // polyline to polyline
                    break;                    
                    
                default:
                    return false;
            }
        }
            
        default:
            return false;
    }
    
    // All tests failed
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// A disk to a line segment
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::DiskToLineSeg(const Disk& disk,
                              const LineSegment& line,
                              const Matrix33& transform,
                              Contact& contact)
{
    // Center shortcut
    Vector2 center  = disk.Transform->Translation();           
    //
    // Get closest point in line
    Vector2 A = line.A;
    transform.TransformVector2(A);
    Vector2 B = line.B;
    transform.TransformVector2(B);
    Vector2 closest = LineToDiskClosestPoint(center, A, B);     
    //
    // Get a vector to it
    Vector2 distVec = center - closest;                         
    
    // Check if we are far enough a return false if so
    if(distVec.Magnitude() >= disk.Radius)
        return false;
    
    // Check if the disk is exactly on the line
    if(distVec.Magnitude() <= 0)
    {
        // Fake a collision then as the side does not play a rolle
        contact.ContactNormal   = (B - A).Perpendicular().Unit();
        contact.Penetration     = disk.Radius;
    }
    else
    {
        // Fill up with the right values
        contact.ContactNormal   = distVec.Unit();
        contact.Penetration     = disk.Radius - distVec.Magnitude();
    }
    
    // Both cases above are collision
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Disk to disk - very basic implementation
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::DiskToDisk(const Disk &diskOne,
                           const Disk &diskTwo,
                           Contact &contact)
{
    // Get normal from the second disk to the first one
    Vector2 normal =    diskOne.Transform->Translation()
                        - diskTwo.Transform->Translation();
    // Get some more info
    float dist = normal.Magnitude();
    float rsum = diskOne.Radius + diskTwo.Radius;
    float diff = rsum - dist;
    
    // Check the distance
    if ( diff > 0 )
    {
        normal.Normalize();
        contact.ContactNormal   = normal;
        contact.Penetration = diff;
        return true;
    }
    
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// Just takes the first one for now
// TODO: See if a more robust way is needed, like max penetration or such.
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::DiskToPolyline(const Disk& disk,
                               const Polyline& polyline,
                               Contact& contact)
{
    for(int i = 0; i < polyline.lines.size(); i++)
    {
        const LineSegment& line = polyline.lines[i];
        
        //
        if(DiskToLineSeg(disk, line, *polyline.Transform, contact))
            return true;
    }
    
    // No line intersected
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Box to disk
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::BoxToDisk(const Box& box, const Disk& disk, Contact& contact)
{
    Matrix33 inv = box.Transform->Inverse();
    //
    Vector2 localCenter = disk.Transform->Translation();
    inv.TransformVector2(localCenter);

    float distx = Absf(localCenter.x);
    float disty = Absf(localCenter.y);
    //
    float ovrlpx = (box.halfWidth + disk.Radius) - distx;
    float ovrlpy = (box.halfHeight + disk.Radius) - disty;
    
    if(ovrlpx <= 0)
        return false;
    
    if(ovrlpy <= 0)
        return false;
    
    if(ovrlpx > ovrlpy)
    {
        contact.Penetration = ovrlpy;
        contact.ContactNormal = localCenter.y <= 0 ?
                                box.Transform->Up() :
                                box.Transform->Up() * -1.0f;
    }
    else
    {
        contact.Penetration = ovrlpx;
        contact.ContactNormal = localCenter.x > 0 ?
                                box.Transform->Right() * -1.0f:
                                box.Transform->Right();
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
// Box to box
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::BoxToBox(const Box& boxOne, const Box& boxTwo, Contact& contact)
{
    return false;
}



// end