////////////////////////////////////////////////////////////////////////////////
//  Intersections.h
//  Furiosity
//
//  Created by Bojan Endrovski on 11/28/12.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "Vector2.h"
#include "Vector3.h"

using std::vector;

namespace Furiosity
{
    /// Intersection between two line segments
    bool LineSegmentsInteresection(const Vector2& p,
                                   const Vector2& pr,
                                   const Vector2& q,
                                   const Vector2& qs,
                                   Vector2& res);

    /// Finds an intersection between a ray and a sphere
    /// if there is one.
    bool RayToSphere(const Vector3& rayOrigin,
                     const Vector3& rayDirection,
                     const Vector3& sphereOrigin,
                     float          sphereRadius,
                     Vector3&       res);
    
    ///
    /// Checks if a point is inside a convex polygon
    ///
    /// @return True if are inside the polygo
    /// @param point Point to check
    /// @param polyVerts The vertices that make up the polygon, in clockwise order
    bool PointInsideConvexPoly(const Vector2& point, const vector<Vector2>& polyVerts);
}
