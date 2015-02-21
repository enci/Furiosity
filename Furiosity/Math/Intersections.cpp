////////////////////////////////////////////////////////////////////////////////
//  Intersections.cpp
//
//  Created by Bojan Endrovski on 11/29/12.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Intersections.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// LineSegmentsInteresection
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::LineSegmentsInteresection(const Vector2 &p,
                                          const Vector2 &pr,
                                          const Vector2 &q,
                                          const Vector2 &qs,
                                          Vector2 &res)
{
    // Get R and S
    Vector2 r = pr - p;
    Vector2 s = qs - q;
    
    // Get the fake 2D cross product
    float rs = r.Cross(s);
    if(rs == 0.0f)  return false;   // TODO: Introduce some tolerance
    
    // Get the params
    Vector2 qp = q - p;
    float t = qp.Cross(s) / rs;
    float u = qp.Cross(r) / rs;
    
    // Check if the params are in the range
    if(t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        res = p + r * t;
        return true;
    }
    else
        return false;
}


////////////////////////////////////////////////////////////////////////////////
// RayToSpere
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::RayToSphere(const Vector3& origin,
                            const Vector3& direction,
                            const Vector3& center,
                            float          radius,
                            Vector3&       res)
{
    // Implementation straight from the vector implicit formulation for a sphere: |c-x|=r
    // and the parametric formulation of a line: p(t)=p0+v*t
    Vector3 d = direction;
    d.Normalize();
    //
    // coefficient a = 1
    float b = 2 * (d.Dot(origin) - d.Dot(center));
    float c =   origin.SquareMagnitude() + center.SquareMagnitude() - (radius*radius)
                - 2 * origin.Dot(center);
    float discriminant = b * b -  4 * c;
    //
    if(discriminant < 0.0f)	// Early out
        return false;
    //
    float t0 = (-b - sqrtf(discriminant)) / 2;
    float t1 = (-b + sqrtf(discriminant)) / 2;
    //
    float t = t0;
    if( (t1 > 0 && t1 < t) || t < 0.0f )
        t = t1;
    //
    if(t <= 0.0f)			// Early out
        return false;
    //
    res = origin + d * t;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Point Inside Convex Poly
// http://stackoverflow.com/questions/1119627/how-to-test-if-a-point-is-inside-of-a-convex-polygon-in-2d-integer-coordinates
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::PointInsideConvexPoly(const Vector2& point,
                                      const vector<Vector2>& polyVerts)
{
    int len = (int)polyVerts.size();
    
    // First translate the polygon so that `point` is the origin. Then, for each
    // edge, get the angle between two vectors: 1) the edge vector and 2) the
    // vector of the first vertex of the edge. If all of the angles are the same
    // sign (which is negative since they will be counter-clockwise) then the
    // point is inside the polygon; otherwise, the point is outside.
    for(int i = 0; i < len; i++)
    {
        Vector2 v1 = polyVerts[i] - point;
        Vector2 v2 = polyVerts[i+1 > len-1 ? 0 : i+1] - point;
        
        Vector2 edge = v1 - v2;
        
        // Note that we could also do this by using the normal + dot product
        // float x = edge.Cross(v1);
        float x = v1.Cross(edge);
        
        // If the point lies directly on an edge then count it as in the polygon
        if (x <= 0)
            return false;
    }
    
    return true;
}



// end