    ////////////////////////////////////////////////////////////////////////////////
//  SpriteRender.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 09/04/2012.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "SpriteRender.h"
//
#include "VertexFormats.h"
#include "ShaderTools.h"
#include "ResourceManager.h"
#include "Entity2D.h"
#include "Defines.h"
#include "logging.h"

using namespace Furiosity;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Renderable from XML
////////////////////////////////////////////////////////////////////////////////
Renderable::Renderable(const XMLElement& settings)
{
    const char* pLayer = settings.Attribute("layer");
    if (pLayer)
        renderLayer = atof(pLayer);
    else
        renderLayer = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
SpriteRender::SpriteRender(const Camera2D* camera,
                           const string& vertShaderFile,
                           const string& fragShaderFile)
:   camera(camera),
    uniforms(0)
{
    shader  = gResourceManager.LoadShader(vertShaderFile, fragShaderFile);
    shader->AddReloadEvent(this, [this](const Resource& shader) { LinkShaders(); });

    LinkShaders();
}


SpriteRender::~SpriteRender()
{
    shader->RemoveReloadEvent(this);
    gResourceManager.ReleaseResource(shader);
    // Cleanup rest
    SafeDeleteArray(uniforms);
}


////////////////////////////////////////////////////////////////////////////////
// Add a renderable object to the render queue. The renderable object will get
// a Render callback.
// @param renderable An object to render
////////////////////////////////////////////////////////////////////////////////
void SpriteRender::AddToRenderer(Renderable* renderable)
{
    renderQueue.push_back(renderable);
}

////////////////////////////////////////////////////////////////////////////////
/// Removes a renderable object from the render queue. The renderable object will
/// no longer get Render callbacks.
/// @param renderable An object not to be rendered anymore
////////////////////////////////////////////////////////////////////////////////
void SpriteRender::RemoveFromRenderer(Renderable* renderable)
{
    for(auto idx = renderQueue.begin(); idx != renderQueue.end(); ++idx)
    {
        if (*idx == renderable)
        {
            renderQueue.erase(idx);
            break;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// Load shaders and connect atributes and uniforms
////////////////////////////////////////////////////////////////////////////////
bool SpriteRender::LinkShaders()
{
    if(uniforms == 0) {
        uniforms = new GLint[NUM_UNIFORMS];
    }

    GLint program = shader->GetProgram();

    // Get uniform locations.    
    uniforms[UNIFORM_PROJECTION]    = glGetUniformLocation(program, "u_projection");
    uniforms[UNIFORM_WORLD]         = glGetUniformLocation(program, "u_world");
    uniforms[UNIFORM_TEXSAMPLER]    = glGetUniformLocation(program, "s_texture");
    uniforms[UNIFORM_TINT]          = glGetUniformLocation(program, "u_tint");
    GL_GET_ERROR();
    
    attribPosition = glGetAttribLocation(program, "a_position");
    attribTexture  = glGetAttribLocation(program, "a_texture");
    GL_GET_ERROR();
    
    return true;
}


void SpriteRender::ActivateShader(const Texture *texture, const Color& tint, const Matrix33& transform)
{    
    GLint program = shader->GetProgram();

    ////////////////////////////////////////////////////////////////////////////////
    // Activate
	// Use shader program
    glUseProgram(program);
    GL_GET_ERROR();
    
    // Validate program before drawing. This is a good check, but only really
    // necessary in a debug build. DEBUG macro must be defined in your debug
    // configurations if that's not already the case.
    #if defined(DEBUG)
        if (!ValidateProgram(program))
        {
            LOG("Failed to validate program (SpriteRender::DrawQuad): %d", program);

            // Crash here, rather than continue and flood the log with error messages.
            assert(false);
            return;
        }
    #endif
    
    // Set alpha blend
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_GET_ERROR();

    
    //glEnable(GL_BLEND);
    // Bind texture
    glActiveTexture(GL_TEXTURE0);                   // Use texture 0
    glBindTexture(GL_TEXTURE_2D, texture->name);    // Work with this texture    
    glUniform1i(uniforms[UNIFORM_TEXSAMPLER], 0);  // Set the sampler to tex 0
    GL_GET_ERROR();
    
    glUniform4f(uniforms[UNIFORM_TINT],
                (float)tint.r / 255.0f,
                (float)tint.g / 255.0f,
                (float)tint.b / 255.0f,
                (float)tint.a / 255.0f);    // Set color
    GL_GET_ERROR();
    
    // Update uniform value
    glUniformMatrix3fv(uniforms[UNIFORM_WORLD],    // Location
                       1,                          // Count
                       0,                          // Transpose (row vs column major)
                       &transform.m11);            // Value
    
    // Update uniform value	    
    Matrix33 proj = camera->Projection();
    // proj.Multiply(transform);
    glUniformMatrix3fv(uniforms[UNIFORM_PROJECTION],    // Location
                       1,                               // Count
                       0,                               // Transpose (row vs column major)
                       &proj.m11);                      // Value
    GL_GET_ERROR();
}


void SpriteRender::DrawQuad(const Matrix33& transform,
                            float width, float height,
                            const Texture* texture,
                            Vector2 offset,
                            Color tint,
                            Vector2 uvFrom,
                            Vector2 uvTo)
{

    ActivateShader(texture, tint, transform);
    
    ////////////////////////////////////////////////////////////////////////////
    //                          Vertices gen
    // Vertices on stack
    static VertexPosition2DTexture vertices[4];
    
    // Half width and half height
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    //
    vertices[0].Position.x = - hw;
    vertices[0].Position.y = - hh;
    vertices[0].Texture = Vector2(uvFrom.x, uvTo.y);
    //
    vertices[1].Position.x = hw;
    vertices[1].Position.y = - hh;
    vertices[1].Texture = uvTo;
    //
    vertices[2].Position.x = - hw;
    vertices[2].Position.y = hh;
    vertices[2].Texture = uvFrom;
    //
    vertices[3].Position.x = hw;
    vertices[3].Position.y = hh;
    vertices[3].Texture = Vector2(uvTo.x, uvFrom.y);
    
    for (int i = 0; i < 4; i++)
        vertices[i].Position += offset;
    
    static GLubyte indices[6] = {0, 1, 2, 1, 3, 2};
    
    
    ////////////////////////////////////////////////////////////////////////////
    // DrawCall
    // Update attribute values
    glVertexAttribPointer(attribPosition,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(VertexPosition2DTexture),
                          &vertices[0].Position);
    glEnableVertexAttribArray(attribPosition);
    GL_GET_ERROR();
    //
    glVertexAttribPointer(attribTexture,
                          2,
                          GL_FLOAT, 
                          GL_FALSE,
                          sizeof(VertexPosition2DTexture),
                          &vertices[0].Texture);
    glEnableVertexAttribArray(attribTexture);
    GL_GET_ERROR();    
    
    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, &indices);
    GL_GET_ERROR();
}

void SpriteRender::DrawRoundedQuad(float radius, int numVerticesPerCorner,
                              const Matrix33& transform,
                              float width,
                              float height,
                              const Texture* texture,
                              Vector2 offset,
                              Color tint,
                              Vector2 uvFrom,
                              Vector2 uvT) {
    const float hw = width * 0.5f;
    const float hh = height * 0.5f;
    
    #ifdef DEBUG
    if(radius > hw || radius > hh) {
        ERROR("SpriteRender::DrawRoundedQuad() The radius cannot exceed the halve width or height.");
    }
    #endif
        
    vector<VertexPosition2DTexture> vertices;
    vertices.reserve(numVerticesPerCorner * 4);
    
    vector<ushort> indices;
    indices.reserve(numVerticesPerCorner * 4);
        
    for(int i = 0; i < 4; ++i)
    {
        Vector2 radiusCenter;
        float radiusOffset = 0;
        
        switch(i) {
            case 0:
                radiusCenter = Vector2(-hw + radius, -hh + radius);
                radiusOffset = Pi;
                break;
            case 1:
                radiusCenter = Vector2(hw - radius, -hh + radius);
                radiusOffset = Pi + HalfPi;
                break;
            case 2:
                radiusCenter = Vector2(hw - radius, hh - radius);
                radiusOffset = 0;
                break;
            case 3:
                radiusCenter = Vector2(-hw + radius, hh - radius);
                radiusOffset = HalfPi;
                break;
        }
        
        const Vector2 uvScale = uvT - uvFrom;
        
        // Radius vertices:
        for(float i = 0; i < HalfPi; i += HalfPi / numVerticesPerCorner)
        {
            Vector2 p(
                cosf(i + radiusOffset) * radius + radiusCenter.x,
                sinf(i + radiusOffset) * radius + radiusCenter.y
            );
            
            VertexPosition2DTexture vertex = {
                p + offset,
                Vector2(
                         p.x * (uvScale.x / width)  + (uvScale.x * 0.5f) + uvFrom.y,
                    -1 * p.y * (uvScale.y / height) + (uvScale.x * 0.5f) + uvFrom.x
                )
            };
            
            vertices.push_back(vertex);
            
            // Push an incremented index:
            indices.push_back(indices.size());
        }
        
    }

    DrawPrimitive(
        GL_TRIANGLE_FAN,
        &vertices[0],
        vertices.size(),
        &indices[0],
        indices.size(),
        texture,
        tint,
        transform);
}

void SpriteRender::DrawEllipse(int numVertices,
        const Matrix33& transform,
        float width,
        float height,
        const Texture* texture,
        Vector2 offset,
        Color tint,
        Vector2 uvFrom,
        Vector2 uvTo) {

	const float hw = width * 0.5f;
	const float hh = height * 0.5f;

	vector<VertexPosition2DTexture> vertices;
	vector<ushort> indices;

	Vector2 uvScale = uvTo - uvFrom;

	int j = 0;
	for(float i = 0; i < TwoPi; i += TwoPi / numVertices, j++) {
		Vector2 v(cosf(i) * hw, sinf(i) * hh);

		VertexPosition2DTexture vertex = {
			v + offset,
			Vector2(
					v.x * (uvScale.x / width)  + (uvScale.x * 0.5f) + uvFrom.y,
			   -1 * v.y * (uvScale.y / height) + (uvScale.x * 0.5f) + uvFrom.x
			      )
		};

		vertices.push_back(vertex);

		indices.push_back(j);
	}

	DrawPrimitive(
	        GL_TRIANGLE_FAN,
	        &vertices[0],
	        vertices.size(),
	        &indices[0],
	        indices.size(),
	        texture,
	        tint,
	        transform);
}

void SpriteRender::DrawPrimitive(uint primitive,
                                 VertexPosition2DTexture *vertices,
                                 ushort vCount,
                                 ushort *indices,
                                 ushort iCount,
                                 const Texture *texture,
                                 Color tint,
                                 const Matrix33& transform)
{
    ActivateShader(texture, tint, transform);
    
    // Call for inheriters
    SetUniforms();
    GL_GET_ERROR();
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // DrawCall
    // Update attribute values
    glVertexAttribPointer(attribPosition,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(VertexPosition2DTexture),
                          &vertices[0].Position);
    glEnableVertexAttribArray(attribPosition);
    GL_GET_ERROR();
    //
    glVertexAttribPointer(attribTexture,
                          2,
                          GL_FLOAT, 
                          GL_FALSE,
                          sizeof(VertexPosition2DTexture),
                          &vertices[0].Texture);
    glEnableVertexAttribArray(attribTexture);
    GL_GET_ERROR();
    
    // Draw
    glDrawElements((GLenum)primitive,
                   iCount,
                   GL_UNSIGNED_SHORT,                   
                   indices);
    GL_GET_ERROR();
}

void SpriteRender::DrawLine(const Vector2& from,
                            const Vector2& to,
                            const Matrix33& transform,
                            float thickness,
                            const Texture* texture,
                            Color tint)
{
    // Get key vectors
    Vector2 diff = to - from;
    Vector2 perp = diff.Perpendicular();
    perp.Normalize();
    perp *= thickness;
    
    vector<VertexPosition2DTexture> vertices =
    {
        { from + perp,  Vector2() },
        { from - perp,  Vector2() },
        { to + perp,    Vector2() },
        { to - perp,    Vector2() }
    };
    
    vector<ushort> indices = {0,1,2,1,2,3};
    
    DrawPrimitive(GL_TRIANGLES,
                  &vertices[0],
                  4,
                  &indices[0],
                  6,
                  texture,
                  tint,
                  transform);
}

void SpriteRender::DrawArc(const Matrix33& transform,
                           float innerRadius,
                           float outerRadius,
                           float from,
                           float to,
                           int slices,
                           const Texture *texture,
                           Color tint)
{
    float t     = from;
    float arc   = to - from;
    float dt    = TwoPi / slices;
    
    // Fill up vertices
    vector<VertexPosition2DTexture> vertices;
    while (arc >= -dt)
    {
        // Position
        Vector2 vec(sinf(t), cosf(t));
        Vector2 inner = vec * innerRadius;
        Vector2 outer = vec * outerRadius;
        
        // UV
        Vector2 innerUV(0.0f, 0.0f);
        Vector2 outerUV(0.0f, 0.0f);
        
        // Vertices
        VertexPosition2DTexture vertexInner;
        vertexInner.Position    = inner;
        vertexInner.Texture     = innerUV;
        //
        VertexPosition2DTexture vertexOuter;
        vertexOuter.Position    = outer;
        vertexOuter.Texture     = outerUV;
        
        // Add to array
        vertices.push_back(vertexInner);
        vertices.push_back(vertexOuter);
        
        // Advance
        if(arc > dt)
            t += dt;
        else
            t += arc;
        //
        arc -= dt;
    }
    
    // Fill up indices
    vector<ushort> indices;;
    int n = (int)(vertices.size() - 1) / 2;
    for (int i = 0; i < n; i++)
    {
        int idx = 2 * i;
        //
        indices.push_back(idx);
        indices.push_back(idx + 2);
        indices.push_back(idx + 1);
        //
        indices.push_back(idx + 1);
        indices.push_back(idx + 2);
        indices.push_back(idx + 3);
    }

    DrawPrimitive(GL_TRIANGLES,
                  &vertices[0],
                  vertices.size(),
                  &indices[0],
                  indices.size(),
                  texture,
                  tint,
                  transform);
}

////////////////////////////////////////////////////////////////////////////////
// RenderQueue
////////////////////////////////////////////////////////////////////////////////
void SpriteRender::RenderQueue()
{
    for (Renderable* r : renderQueue)
        r->Render(this);
}



// eof
