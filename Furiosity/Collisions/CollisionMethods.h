////////////////////////////////////////////////////////////////////////////////
//  CollisionMethods.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/23/11.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_METHODS_H
#define COLLISION_METHODS_H

#include "CollisionShapes.h"
#include "Contact.h"

namespace Furiosity
{    
    ///
    /// CollsionShapeToWall
    /// Entry point as it works for all shapes, hence all the pointers. Rest of the
    /// code loves references!
    ///
    bool CollisionShapeToLineSeg(CollisionShape const*  shape,
                                 LineSegment const*     line,
                                 Contact*               contact);
    
    
    ///
    /// Disk to a line segment that can be transformed as needed
    ///
    bool DiskToLineSeg(const Disk& disk,
                       const LineSegment& line,
                       const Matrix33& transform,
                       Contact& contact);

    
    ///
    /// Any shape to any shape
    /// An entry point that works for all shapes and then delegates the call
    /// to the actual test
    ///
    bool ShapeToShape(CollisionShape const* shapeOne,
                      CollisionShape const* shapeTwo,
                      Contact* contact);
    
    
    ///
    /// DiskToPolyline
    ///
    bool DiskToPolyline(const Disk& disk, const Polyline& polyline, Contact& contact);
    
    
    ///
    /// Disk to disk
    /// The simplest and cheapest test
    ///
    bool DiskToDisk(const Disk& diskOne, const Disk& diskTwo, Contact& contact);
    
    
    ///
    /// Box to disk
    ///
    bool BoxToDisk(const Box& box, const Disk& disk, Contact& contact);
    
    
    ///
    /// Box to box
    ///
    bool BoxToBox(const Box& boxOne, const Box& boxTwo, Contact& contact);
}

#endif