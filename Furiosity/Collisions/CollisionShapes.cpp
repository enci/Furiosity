////////////////////////////////////////////////////////////////////////////////
//  CollisionShapes.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/21/11.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "CollisionShapes.h"

#include "DebugDraw2D.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CollisionShape* CollisionShape::Create(const Matrix33 *transform, 
                                       const XMLElement *settings)
{
    const XMLElement* pDisk = settings->FirstChildElement("disk");
    if(pDisk)
    {
        const char* pRadius = pDisk->Attribute("radius");
        assert(pRadius);
        return new Disk(transform, atof(pRadius));
    }
    
    const XMLElement* pBox = settings->FirstChildElement("box");
    if(pBox)
    {
        const char* pWidth = pBox->Attribute("width");
        const char* pHeight = pBox->Attribute("height");
        //
        assert(pWidth && pHeight);
        //
        return new Box(transform, atof(pWidth), atof(pHeight));
    }
    
    // TODO: Add more shapes here if needed
    
    // Fail, nothing found here
    assert(0);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//                         - POLYLINE -
////////////////////////////////////////////////////////////////////////////////
Polyline::Polyline(const Matrix33* transform, const std::vector<Vector2>& points)
    : CollisionShape(transform, -1,  COLLISION_SHAPE_POLYLINE)
{
    // Calc radius as max distance to points
    float dist = 0;
    for(int i = 0; i < points.size(); i++)
    {
        if(points[i].Magnitude() > dist)
            dist = points[i].Magnitude();
    }
    Radius = dist;
    
    // Create line segments
    int i;
    for(i = 0; i < points.size() - 1; i++)
        // From every points to the next one
        lines.push_back( LineSegment(points[i], points[i + 1]) );
    // Connect the last one and the first one
    lines.push_back(LineSegment(points[i], points[0]));
}


////////////////////////////////////////////////////////////////////////////////
//                             - BOX -
////////////////////////////////////////////////////////////////////////////////
Box::Box(const Matrix33* transform, float width, float height)
: CollisionShape(transform, -1, COLLISION_SHAPE_BOX)
{
    halfWidth = width * 0.5f;
    halfHeight = height * 0.5f;
    Radius = sqrtf( (halfWidth * halfWidth) + (halfHeight * halfHeight) );
}


#ifdef DEBUG

////////////////////////////////////////////////////////////////////////////////
//
//                          - ALL THE DEBUG RENDER -
//
////////////////////////////////////////////////////////////////////////////////

void Disk::DebugRender(Color c)
{
    gDebugDraw2D.AddCircle(Transform->Translation(), Radius, c);
}

void LineSegment::DebugRender(Color c)
{
    gDebugDraw2D.AddLine(A, B, c);
}

void Polyline::DebugRender(Color c)
{
    for(int i = 0; i < lines.size(); i++)
    {
        Vector2 A = lines[i].A;
        Transform->TransformVector2(A);
        //
        Vector2 B = lines[i].B;
        Transform->TransformVector2(B);
        //
        gDebugDraw2D.AddLine(A, B, c);
    }
}

void Box::DebugRender(Color c)
{
    // Calculate vertices in local space
    Vector2 A(halfWidth, -halfHeight);
    Vector2 B(halfWidth, halfHeight);
    Vector2 C(-halfWidth, halfHeight);
    Vector2 D(-halfWidth, -halfHeight);
    
    // Transform to world coordinates
    Transform->TransformVector2(A);
    Transform->TransformVector2(B);
    Transform->TransformVector2(C);
    Transform->TransformVector2(D);
    
    
    // Draw them
    gDebugDraw2D.AddLine(A, B, c);
    gDebugDraw2D.AddLine(B, C, c);
    gDebugDraw2D.AddLine(C, D, c);
    gDebugDraw2D.AddLine(D, A, c);
}

#endif

