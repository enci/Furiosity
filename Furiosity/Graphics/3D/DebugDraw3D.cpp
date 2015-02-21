////////////////////////////////////////////////////////////////////////////////
//  DebugDraw3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/4/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "DebugDraw3D.h"

// Local includes
#include "ShaderTools.h"
#include "ResourceManager.h"
#include "Defines.h"
#include "Effect.h"

using namespace Furiosity;

// Actualy creates a new DebugDraw, but the real work is in the Init not the ctor
// This serves as a manager
DebugDraw3D Furiosity::gDebugDraw3D;

////////////////////////////////////////////////////////////////////////////////
// No args constructor
////////////////////////////////////////////////////////////////////////////////
DebugDraw3D::DebugDraw3D()
{
    // Set values
    linesCount = 0;
    pointsCount = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////
DebugDraw3D::~DebugDraw3D() {}

////////////////////////////////////////////////////////////////////////////////
// Initialize - Acts as a constructor
////////////////////////////////////////////////////////////////////////////////
void  DebugDraw3D::Initialize(Camera3D& camera)
{
    this->camera = &camera;
    shader = unique_ptr<Effect>(new Effect("/SharedResources/Shaders/DebugDraw3D.vsh",
                                           "/SharedResources/Shaders/DebugDraw3D.fsh"));
    
    attribColor = (*shader)->GetAttribute("a_color");
    attribVertex = (*shader)->GetAttribute("a_position");
    paramCamera = (*shader)->GetParameter("u_worldviewproj");
}

////////////////////////////////////////////////////////////////////////////////
// Draw
////////////////////////////////////////////////////////////////////////////////
void DebugDraw3D::Draw()
{
    (*shader)->Activate();
    
    // Validate program before drawing
#if defined(DEBUG)    
    if(!(*shader)->Validate())
        return;
#endif
	
    // Update uniform value
    Matrix44 mvp = camera->Projection() * camera->View();
    paramCamera->SetValue(mvp);
    
    if(linesCount > 0)
    {
        attribVertex->SetAttributePointer(3,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          sizeof(VertexPosition3DColor),
                                          &vertexArray[0].Position);
        //
        attribColor->SetAttributePointer(4,
                                         GL_UNSIGNED_BYTE,
                                         GL_TRUE,
                                         sizeof(VertexPosition3DColor),
                                         &vertexArray[0].Color);
        //
        glDrawArrays(GL_LINES, 0, linesCount * 2);
        GL_GET_ERROR();
    }
		   
	// Draw points too (try the same shader)
	if (pointsCount > 0)
    {
        attribVertex->SetAttributePointer(3,
                                          GL_FLOAT,
                                          GL_FALSE,
                                          sizeof(VertexPosition3DColor),
                                          &pointArray[0].Position);
		//
        attribColor->SetAttributePointer(4,
                                         GL_UNSIGNED_BYTE,
                                         GL_TRUE,
                                         sizeof(VertexPosition3DColor),
                                         &pointArray[0].Color);
		//
		glDrawArrays(GL_POINTS, 0, pointsCount);
        GL_GET_ERROR();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Clear
////////////////////////////////////////////////////////////////////////////////
void DebugDraw3D::Clear()
{
	linesCount = 0;
	pointsCount = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Add line
////////////////////////////////////////////////////////////////////////////////
void DebugDraw3D::AddLine(const Vector3& from, const Vector3& to, const Color& color)
{
	if (linesCount < maxLines)
    {
		vertexArray[linesCount * 2].Position     = from;
		vertexArray[linesCount * 2 + 1].Position = to;
        //
        vertexArray[linesCount * 2].Color       = color;
		vertexArray[linesCount * 2 + 1].Color   = color;
        //
		++linesCount;
	}
    // else ignore
}

////////////////////////////////////////////////////////////////////////////////
// AddCircle
////////////////////////////////////////////////////////////////////////////////
void DebugDraw3D::AddCircle(const Vector3& center,
                            float radius,
                            const Color& color,
                            int divs)
{
	float dt = Pi / (float)divs;
	float t = 0.0f;
	
	Vector3 v0(center.x + radius * cos(t), center.y, center.z + radius * sin(t));
	for (; t < TwoPi - dt; t += dt)
	{
		Vector3 v1(center.x + radius * cos(t + dt), center.y, center.z + radius * sin(t + dt));
		//
		AddLine(v0, v1, color);
		v0 = v1;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Add Sphere
////////////////////////////////////////////////////////////////////////////////
void DebugDraw3D::AddAxisSphere(const Vector3& center,
                                float radius,
                                const Color& xzColor,
                                const Color& xyColor,
                                const Color& yzColor,
                                int divs)
{
	float dt = Pi / (float)divs;
	float t = 0.0f;
	
	Vector3 vh0(center.x + radius * cos(t), center.y, center.z + radius * sin(t));
	for (; t < TwoPi - dt; t += dt)
	{
		Vector3 vh1(center.x + radius * cos(t + dt),
                    center.y,
                    center.z + radius * sin(t + dt));
		//
		AddLine(vh0, vh1, xzColor);
		vh0 = vh1;
	}
    
    Vector3 vv0(center.x + radius * cos(t),
                center.y + radius * sin(t),
                center.z);
    t = 0.0f;
	for (; t < TwoPi - dt; t += dt)
	{
		Vector3 vv1(center.x + radius * cos(t + dt),
                    center.y + radius * sin(t + dt),
                    center.z);
		//
		AddLine(vv0, vv1, xyColor);
		vv0 = vv1;
	}
    
    vv0 = Vector3(center.x,
                  center.y + radius * sin(t),
                  center.z + radius * cos(t));
    t = 0.0f;
	for (; t < TwoPi - dt; t += dt)
	{
		Vector3 vv1(center.x,
                    center.y + radius * sin(t + dt),
                    center.z + radius * cos(t + dt));
		//
		AddLine(vv0, vv1, yzColor);
		vv0 = vv1;
	}

}


////////////////////////////////////////////////////////////////////////////////
// Add Axis
////////////////////////////////////////////////////////////////////////////////
void DebugDraw3D::AddAxis(const Matrix44 &trans, float size)
{
    Vector3 x(size, 0, 0);
    Vector3 y(0, size, 0);
    Vector3 z(0, 0, size);
    Vector3 o(0, 0, 0);
    //
    x = trans * x;
    y = trans * y;
    z = trans * z;
    o = trans * o;
    //
    AddLine(o, x, Color::Red);          // X is red
    AddLine(o, y, Color::Green);        // Y is greeen
    AddLine(o, z, Color::Blue);         // Z is blue
}

////////////////////////////////////////////////////////////////////////////////
// Queues a 2d point for drawing
////////////////////////////////////////////////////////////////////////////////
void DebugDraw3D::AddPoint(const Vector3& position, const Color& color)
{
	if (pointsCount < maxPoints)
	{
		pointArray[pointsCount].Position = position;
		pointArray[pointsCount].Color = color;
		//
		++pointsCount;
	}
    // else ignore
}

