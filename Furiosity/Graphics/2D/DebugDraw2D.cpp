////////////////////////////////////////////////////////////////////////////////
//  DebugDraw2D.cpp
//
//  Created by Bojan Endrovski on 18/03/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

#include "DebugDraw2D.h"

// Local includes
#include "ShaderTools.h"
#include "ResourceManager.h"
#include "Defines.h"
#include "Effect.h"

using namespace Furiosity;

// Actualy creates a new DebugDraw, but the real work is in the Init not the ctor
// This serves as a manager
DebugDraw2D Furiosity::gDebugDraw2D;

////////////////////////////////////////////////////////////////////////////////
// No args constructor
////////////////////////////////////////////////////////////////////////////////
DebugDraw2D::DebugDraw2D()
{
    // Set values
    linesCount = 0;
	pointsCount = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////
DebugDraw2D::~DebugDraw2D() {}

////////////////////////////////////////////////////////////////////////////////
// Initialize - Acts as a constructor
////////////////////////////////////////////////////////////////////////////////
void  DebugDraw2D::Initialize(const Camera2D& camera)
{
    this->camera = &camera;
    shader = unique_ptr<Effect>(new Effect("/SharedResources/Shaders/DebugDraw2D.vsh",
                                           "/SharedResources/Shaders/DebugDraw2D.fsh"));
    
    attribColor = (*shader)->GetAttribute("color");
    attribVertex = (*shader)->GetAttribute("position");
    paramCamera = (*shader)->GetParameter("u_projection");
}

////////////////////////////////////////////////////////////////////////////////
// Draw
////////////////////////////////////////////////////////////////////////////////
void DebugDraw2D::Draw()
{
    (*shader)->Activate();
    
    // Validate program before drawing
#if defined(DEBUG)
    if(!(*shader)->Validate())
        return;
#endif

	
    // Update uniform value
    Matrix33 proj = camera->Projection();
    paramCamera->SetValue(proj);
    
    if(linesCount > 0)
    {	
        // Update attribute values
        attribVertex->SetAttributePointer(2, GL_FLOAT, 0, 0, vertexArray);
        attribColor->SetAttributePointer(4, GL_UNSIGNED_BYTE, 1, 0, vertexColors);
        // Draw
        glDrawArrays(GL_LINES, 0, linesCount * 2);
        GL_GET_ERROR();
    }	
	
	// Draw points too (try the same shader)
	if (pointsCount > 0)
    {
		attribVertex->SetAttributePointer(2, GL_FLOAT, 0, 0, pointArray);
		attribColor->SetAttributePointer(4, GL_UNSIGNED_BYTE, 1, 0, pointColors);
		glDrawArrays(GL_POINTS, 0, pointsCount);
        GL_GET_ERROR();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Clear
////////////////////////////////////////////////////////////////////////////////
void DebugDraw2D::Clear()
{
	linesCount = 0;
	pointsCount = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Add line
////////////////////////////////////////////////////////////////////////////////
void DebugDraw2D::AddLine(const Vector2& from, const Vector2& to, const Color& color)
{
	if (linesCount < maxLines)
    {		
		vertexArray[linesCount * 2]         = from;
		vertexArray[linesCount * 2 + 1]     = to;
		//
		vertexColors[linesCount * 2]        = color;
		vertexColors[linesCount * 2 + 1]	= color;
        //
		++linesCount;
	}
    // else ignore
}

////////////////////////////////////////////////////////////////////////////////
// AddCircle
////////////////////////////////////////////////////////////////////////////////
void DebugDraw2D::AddCircle(const Vector2& center,
                            float radius,
                            const Color& color,
                            int divs)
{
	float dt = Pi / (float)divs;
	float t = 0.0f;
	
	Vector2 v0(center.x + radius * cos(t), center.y + radius * sin(t));
	for (; t < TwoPi - dt; t += dt)
	{
		Vector2 v1(center.x + radius * cos(t + dt), center.y + radius * sin(t + dt));
		//
		AddLine(v0, v1, color);
		v0 = v1;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Queues a 2d point for drawing 
////////////////////////////////////////////////////////////////////////////////
void DebugDraw2D::AddPoint(const Vector2& position, const Color& color)
{
	if (pointsCount < maxPoints) 
	{
		pointArray[pointsCount]     = position;
		pointColors[pointsCount]    = color;
		//
		++pointsCount;
	}
    // else ignore
}

////////////////////////////////////////////////////////////////////////////////
// Add a rectangle
////////////////////////////////////////////////////////////////////////////////
void DebugDraw2D::AddRectangle(const Vector2& min, const Vector2& max, const Color& color)
{
    AddLine(min, Vector2(min.x, max.y), color);
    AddLine(min, Vector2(max.x, min.y), color);
    AddLine(max, Vector2(min.x, max.y), color);
    AddLine(max, Vector2(max.x, min.y), color);
}

#endif // DEBUG




