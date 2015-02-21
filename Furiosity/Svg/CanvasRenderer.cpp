////////////////////////////////////////////////////////////////////////////////
//  CanvasRenderer.cpp
//
//  Created by Gerard Meier on 7/19/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "CanvasRenderer.h"
#include "Resource.h"
#include "VertexFormats.h"

using namespace Furiosity;

CanvasRenderer::CanvasRenderer(const Camera2D* camera,
    const std::string& vertexShader, const std::string& fragmentShader,
    GLuint glPrimitive)
: camera(camera)
, glPrimitive(glPrimitive)
{
    shader = gResourceManager.LoadShader(vertexShader, fragmentShader);
    
    // Perticularly usefull for Android. Shares require a recompile.
    // shader->AddReloadEvent(this, &CanvasRenderer::ShaderReloaded);
    
    LinkShaders();
}

CanvasRenderer::~CanvasRenderer()
{
    shader->RemoveReloadEvent(this);
    gResourceManager.ReleaseResource(shader);
}

void CanvasRenderer::LinkShaders()
{
    GLuint program = shader->GetProgram();

    // Retrieve uniform and attribute handlers from the shader:
    uniformProjection = glGetUniformLocation(program, "u_projection");
    GL_GET_ERROR();
    uniformWorld      = glGetUniformLocation(program, "u_world");
    GL_GET_ERROR();
    attribPosition    = glGetAttribLocation(program, "a_position");
    GL_GET_ERROR();
    attribColor       = glGetAttribLocation(program, "a_color");
    GL_GET_ERROR();
}

void CanvasRenderer::Render(const Matrix33& transform, Canvas* resource)
{
    const Matrix33& proj = camera->Projection();
    
    GLuint program = shader->GetProgram();
    glUseProgram(program);
    GL_GET_ERROR();
    
    #if defined(DEBUG)
    if (!ValidateProgram(program))
    {
    	LOG("Failed to validate program (SpriteRender::ActivateShader): %d", program);
    	GL_GET_ERROR();
        return;
    }
    
    if(resource->vbo[0] == -1 || resource->vbo[1] == -1) {
        ERROR("Trying to render a non Compiled SvGResource.");
    }
    
    #endif
    
    // Local transform:
    glUniformMatrix3fv(uniformWorld, 1, 0, &transform.m11);
    GL_GET_ERROR();

    // Camera:
    glUniformMatrix3fv(uniformProjection, 1, 0, &proj.m11);
    GL_GET_ERROR();
    

    glBindBuffer(GL_ARRAY_BUFFER, resource->vbo[0]);
    GL_GET_ERROR();
    
    
    glVertexAttribPointer(  attribPosition,                 // The attribute in the shader.
                            2,                              // Number of "fields", in this case 2 floats X & Y.
                            GL_FLOAT,                       // Data type
                            GL_FALSE,                       // Must these values be normalized? No tanks.
                            sizeof(VertexPosition2DColor),  // Size of each structure
                            0                               // Offset
    );
    GL_GET_ERROR();
    glEnableVertexAttribArray(attribPosition);
    GL_GET_ERROR();
    
    glVertexAttribPointer(  attribColor,                    // The attribute in the shader.
                            4,                              // Number of "fields", in this case 4 bytes (RGBA).
                            GL_UNSIGNED_BYTE,               // The type of each "field".
                            GL_TRUE,                        // Must the values be normalized? yea - a byte != float.
                            sizeof(VertexPosition2DColor),  // Size of each structure.
                            (void*) sizeof(Vector2)         // The pointer address is interpreted as offset.
    );
    GL_GET_ERROR();
    glEnableVertexAttribArray(attribColor);
    GL_GET_ERROR();
    
    
    // Bind vertex index buffer:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->vbo[1]);

    // TODO: once we have proper triangulation, this will work:
    glDrawElements(
        glPrimitive,                // GL primitive type
        (int)resource->indices.size(),   // How many indices to draw
        GL_UNSIGNED_SHORT,          // Data type of indices
        0                           // Offset
    );

    // Disable attributes, we've never done this before. Doing this makes sure
    // some global state doesn't leak into the next set of gl calls.
    glDisableVertexAttribArray(attribPosition);
    glDisableVertexAttribArray(attribColor);
    GL_GET_ERROR();

    // Unbind buffers:
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_GET_ERROR();
}
