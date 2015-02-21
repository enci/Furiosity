////////////////////////////////////////////////////////////////////////////////
//  Canvas.cpp
//
//  Created by Gerard Meier on 7/19/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Canvas.h"
#include <limits>
#include "Triangulate.h"

using namespace Furiosity;

Canvas::Canvas()
{
    vbo[0] = vbo[1] = -1;
    
    // Default Canvas does nothing, nor can it be rendered.
}

Canvas::~Canvas()
{
    glDeleteBuffers(2, vbo);
    vbo[0] = vbo[1] = -1;
}

bool Canvas::IsValid() {
    return glIsBuffer(vbo[0] && glIsBuffer(vbo[1]));
}

void Canvas::Invalidate()
{
    glDeleteBuffers(2, vbo);
    vbo[0] = vbo[1] = -1;
}

void Canvas::Compile()
{
    
    if(vbo[0] != -1 || vbo[1] != -1)
    {
        ERROR("vgResource::Compile() - the VBOs are already uploaded. Cannot call twice.");
        return;
    }

    if(vertices.empty()) {
        ERROR("Cannot upload canvas without vertices.");
        return;
    }
    
    if(indices.empty()) {
        ERROR("Cannot upload canvas without indices.");
        return;
    }
    
    // From this point onwards, we upload all vertices:
    
    glGenBuffers(2, &vbo[0]);
    GL_GET_ERROR();
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    GL_GET_ERROR();
    // Copy into VBO:
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPosition2DColor) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    GL_GET_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer
    GL_GET_ERROR();
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    GL_GET_ERROR();
    // Copy into VBO:
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indices.size(), &indices[0], GL_STATIC_DRAW);
    GL_GET_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind buffer
    GL_GET_ERROR();
}


void Canvas::GenerateVertices(std::vector<Vector2>& buffer,
                              const Color& a,
                              const Color& b,
                              const float& angle)
{
    // TODO: Create simple logic for these special cases:
    //      angle == 0
    //      a == b
    //      angle == Pi
    //
    // TODO: Perhaps we can rotate the gradient instead of each vertex?
    
    // Storage container for the rotated vertices:
    std::vector<Vector2> rotated;
    rotated.reserve(buffer.size());

    Matrix33 m;
    m.SetRotation(angle);
    
    // Initial extreme values.
    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();
    
    // Rotate, and find minima / maxima
    for(size_t i = 0; i < buffer.size(); ++i)
    {
        // Rotate vector to the gradient's angle:
        Vector2 p = buffer.at(i);
        m.TransformVector2(p);
        rotated.push_back(p);
        
        // Find extremes:
        max = std::max(max, p.y);
        min = std::min(min, p.y);
    }
    
    // This works b.c. "minus minus" equals "plus".
    const float height = fabs(max - min);
    
    // Optimize common calculation:
    const float oneOverHeight = 1.0f / height;
    
    // Account for negative coordinates:
    const float o = (min < 0) ? oneOverHeight * fabs(min) : 0;

    // Translate the position:
    
    // const float oneOverMin = oneOverHeight * min;
    
    for(size_t i = 0; i < buffer.size(); ++i)
    {
        // We only interpolate over the Y axis, since the vertices
        // have been rotated. I bet there are better ways.
        float delta = oneOverHeight * rotated.at(i).y + o;// - oneOverMin;
        
        // The actual vertex that the GPU's shader expects:
        VertexPosition2DColor v = { buffer.at(i), Lerp(a, b, delta) };
        
        if(delta < 0 || delta > 1)
        {
            LOG("Color lerp delta not in [0..1]");
            LOG("delta: %f a: #%x, b: #%x, lerp: #%x", delta, a.integervalue, b.integervalue, v.color.integervalue);
            LOG("Polygon min: %f, max: %f, height: %f oneover: %f, overmin: %f", min, max, height, oneOverHeight, oneOverHeight * min);
        }
        
        vertices.push_back(v);
    }
}

void Canvas::Reload()
{
    ERROR("Sorry Android, this isn't programmed yet.");
    
    // TODO: re add all vertices, then call:
    // Compile();
}


////////////////////////////////////////////////////////////////////////////////
/// Calls for some sort of some ad-hoc drawing API.
////////////////////////////////////////////////////////////////////////////////


/// Start a new polygon
Canvas& Canvas::Begin()
{
    // Clear the work-in-progress buffer:
    buffer.clear();
    
    return *this;
}

/// Draw a directline
Canvas& Canvas::Vertex(const Vector2& p)
{

    // Detect duplicate values, the triangulation code fails if there
    // are any.
    for(const Vector2& v : buffer)
    {
        if(v == p)
        {
            return *this;
        }
    }

    buffer.push_back(p);
    
    return *this;
}

/// Fill with a solid color:
Canvas& Canvas::Fill(const Color& color)
{
    return Fill(color, color, 0);
}

/// Fill with a linear gradient:
Canvas& Canvas::Fill(const Color& a, const Color& b, const float& angle)
{
    if(buffer.empty())
    {
        ERROR("There are no vertices to be rendered.");
    }
    
    // Offset, required for the indices.
    const size_t offset = vertices.size();
    
    // Triangulate and append the indices back into "all the indices":
    for(const unsigned short i : Triangulate::Process(buffer))
    {
        indices.push_back(i + offset);
    }

    // Colorize:
    GenerateVertices(buffer, a, b, angle);
    
    return *this;
}

Canvas& Canvas::Stroke(const float& border, const Color& a, const float& angle)
{
    return Stroke(border, a, a, angle);
}

Canvas& Canvas::Stroke(const float& border, const Color& colorBegin, const Color& colorEnd, const float& angle)
{ //return *this;
    // To hold the shrunk polygon:
    std::vector<Vector2> shrunk;
    
    // Flip, that way the border is on the inside.
    const float w = border * -1;
    
    for(size_t i = 0; i < buffer.size(); ++i)
    {
        // Vertices:
        const Vector2& a = (i == 0) ? buffer.at(buffer.size() - 1) : buffer.at(i - 1);
        const Vector2& b = buffer[i];
        const Vector2& c = (i == buffer.size() - 1) ? buffer.at(0) : buffer.at(i + 1);
        
        // Direction of each edge:
        const Vector2 ab = b - a;
        const Vector2 bc = c - b;
        
        Vector2 perp1 = ab.Perpendicular(); perp1.Normalize(); perp1 *= w;
        Vector2 perp2 = bc.Perpendicular(); perp2.Normalize(); perp2 *= w;
        
        // Runs from A to B.
        const Vector2 beam1A = a + perp1;
        const Vector2 beam1B = b + perp1;
        
        // B to C
        const Vector2 beam2A = b + perp2;
        const Vector2 beam2B = c + perp2;
        
        const Vector2 cross = LineIntersection(beam1A, beam1B, beam2A, beam2B);
        
        shrunk.push_back(cross);
        
        //LOG("a = segment(%f, %f, %f, %f)", beam1A.x, beam1A.y, beam1B.x, beam1B.y);
        //LOG("b = segment(%f, %f, %f, %f)", beam2A.x, beam2A.y, beam2B.x, beam2B.y);
        
        //LOG("Crossing: x:%f y:%f", cross.x, cross.y);
    }
    
    std::vector<Vector2> result;
    
    // Generate triangles from the shrunk shape:
    for(size_t i = 0; i < shrunk.size(); ++i)
    {
        // Vertices:
        const Vector2& a = (i == 0) ? buffer.at(buffer.size() - 1) : buffer.at(i - 1);
        const Vector2& b = buffer.at(i);
        const Vector2& cross = shrunk[i];
        const Vector2& nextCross = (i == shrunk.size() - 1) ? shrunk.at(0) : shrunk.at(i + 1);

        // TODO: we're pushing _some_ vertices double. Later on we could
        // optimize this.
        result.push_back(a);         // 0
        result.push_back(b);         // 1
        result.push_back(cross);     // 2
        result.push_back(nextCross); // 3
        
        // Desired index sequence:
        // 0 1 2 2 3 1
        
        const unsigned short offset = i * 4 + vertices.size();
        indices.push_back(offset + 0);
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);
        indices.push_back(offset + 2);
        indices.push_back(offset + 3);
        indices.push_back(offset + 1);
    }

    GenerateVertices(result, colorBegin, colorEnd, angle); // TODO: angle.
 
    return *this;
}

Canvas& Canvas::Bezier(const std::vector<Vector2>& points)
{
    // TODO: fancy interval calculation.
    const float interval = 1.0f / 30.0f;
    
    for(float i = interval; i <= 1; i += interval) {
        Vertex(
            BezierCurve(points, i)
        );
    }

    return *this;
}
