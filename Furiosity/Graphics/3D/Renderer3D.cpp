////////////////////////////////////////////////////////////////////////////////
//  Renderer.cpp
//
//  Created by Bojan Endrovski on 09/04/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Renderer3D.h"

// Local includes
#include "Furiosity.h"
#include "ResourceManager.h"    // Read file stuff
#include "ShaderTools.h"
#include "Frmath.h"
#include "Color.h"


using namespace std;
using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//                              - Renderable -
////////////////////////////////////////////////////////////////////////////////
Renderable3D::Renderable3D(/*Renderer3D& renderer,
                           float       layer */) // :
//    renderer(renderer),
//    layer(layer)
{
//    renderer.AddToRenderer(this);
}

Renderable3D::~Renderable3D()
{
//    renderer.RemoveFromRenderer(this);
}


////////////////////////////////////////////////////////////////////////////////
//
//                             - Renderer3D -
//
////////////////////////////////////////////////////////////////////////////////


/*

////////////////////////////////////////////////////////////////////////////////
// Renderer3D
////////////////////////////////////////////////////////////////////////////////
Renderer3D::Renderer3D(const Camera3D&   camera,
                       const string& vertShaderFile,
                       const string& fragShaderFile)
:   camera(camera),
//    uniforms(NUM_UNIFORMS),
    effect(vertShaderFile, fragShaderFile),
    worldViewProjParam(*effect->GetParameter("u_worldviewproj")),
    normalMatrixParam(*effect->GetParameter("u_normalmatrix")),
    lightDirectionParam(*effect->GetParameter("u_lightdir")),
    ambientParam(*effect->GetParameter("u_ambient")),
    textureParam(*effect->GetParameter("s_texture")),
    attribPosition(*effect->GetAttribute("a_position")),
    attribNormal(*effect->GetAttribute("a_normal")),
    attribTexture(*effect->GetAttribute("a_texture"))
{
    lightDir = Vector3(0, 1, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Dtor
////////////////////////////////////////////////////////////////////////////////
Renderer3D::~Renderer3D() {}

////////////////////////////////////////////////////////////////////////////////
// Add a renderable object to the render queue. The renderable object will get
// a Render callback.
// @param renderable An object to render
////////////////////////////////////////////////////////////////////////////////
void Renderer3D::AddToRenderer(Renderable3D* renderable)
{
    renderQueue.push_back(renderable);
}

////////////////////////////////////////////////////////////////////////////////
/// Removes a renderable object from the render queue. The renderable object will
/// no longer get Render callbacks.
/// @param renderable An object not to be rendered anymore
////////////////////////////////////////////////////////////////////////////////
void Renderer3D::RemoveFromRenderer(Renderable3D* renderable)
{
    vector<Renderable3D*>::iterator idx;
    for (idx = renderQueue.begin(); idx != renderQueue.end(); ++idx)
    {
        if (*idx == renderable)
        {
            renderQueue.erase(idx);
            break;
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
// Renders all the objects in the render queue
////////////////////////////////////////////////////////////////////////////////

void Renderer3D::RenderQueue()
{
    vector<Renderable3D*> sorted = renderQueue;
    std::sort(sorted.begin(), sorted.end(),
              [](const Renderable3D* lhs, const Renderable3D* rhs)
              {
                  return lhs->RenderLayer() < rhs->RenderLayer();
              });
    
    for (Renderable3D* r : sorted)
        r->Render(*this);
}


////////////////////////////////////////////////////////////////////////////////
// Render
////////////////////////////////////////////////////////////////////////////////
void Renderer3D::Render(const Matrix44& transform,
                        ModelMesh3D&    model,
                        Texture&        texture,
                        const Color&    ambient)
{
    // Use shader program
    glUseProgram(effect->GetProgram());
    GL_GET_ERROR();
    
    // Set light
    lightDir.Normalize();
    lightDirectionParam.SetValue(lightDir);
    
    // Set ambient
    Vector3 ambientvec(ambient.r, ambient.g, ambient.b);
    ambientvec *= 1.0f / 255.0f;
    ambientParam.SetValue(ambientvec);

    
    // Set world view projection
    Matrix44 wvp = camera.Projection() * camera.View() * transform;
    worldViewProjParam.SetValue(wvp);    
    
    // Set normal transformation matrix
    // Matrix33 normalMtx = (camera.View() * transform).GetMatrix33();
    Matrix33 normalMtx = (transform).GetMatrix33();
    normalMtx.Invert();
    normalMtx.Transpose();
    normalMatrixParam.SetValue(normalMtx);    
    
    textureParam.SetValue(texture);
    
    ////////////////////////////////////////////////////////////////////////////////


    const bool useVbo = model.HasVertexBuffers();

    // Default to VBO values, the pointer addresses are interpreted as byte-offsets.
    const void* firstPosition = (void*) offsetof(VertexPositionNormalTexture, Position);
    const void* firstNormal   = (void*) offsetof(VertexPositionNormalTexture, Normal);
    const void* firstTexture  = (void*) offsetof(VertexPositionNormalTexture, Texture);
    const void* firstIndex    = 0;

    // We are not using VBO, provide actual pointers to data instead of byte-offsets.
    if(!useVbo)
    {
    	firstPosition = &model.Vertices()[0].Position;
    	firstNormal   = &model.Vertices()[0].Normal;
    	firstTexture  = &model.Vertices()[0].Texture;
    	firstIndex    = model.Indices();
    }
    else
    {
    	const GLuint* vbo = model.VertexBuffers();

#ifdef DEBUG
    	// It's typical during Android development that buffers break.
    	if(glIsBuffer(vbo[0]) != GL_TRUE) {
    		ERROR("The vertex buffer is not a valid buffer (VBO). File: %s.", model.Path().c_str());
    	}

    	if(glIsBuffer(vbo[1]) != GL_TRUE) {
			ERROR("The index buffer is not a valid buffer (VBO). File: %s.", model.Path().c_str());
		}
#endif

        // Bind the buffers to the global state
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    }


    GLsizei size = sizeof(VertexPositionNormalTexture);
    attribPosition.SetAttributePointer( 3, GL_FLOAT, GL_FALSE, size, firstPosition);
    attribNormal.SetAttributePointer(   3, GL_FLOAT, GL_FALSE, size, firstNormal);
    attribTexture.SetAttributePointer(  2, GL_FLOAT, GL_FALSE, size, firstTexture);
    
	// Validate just before drawing. If the shader has errors, then this call will find them.
#if defined(DEBUG)
    if (!ValidateProgram(effect->GetProgram()))
    {
        LOG("Failed to validate program: %d", effect->GetProgram());
        return;
    }
#endif

	glDrawElements(GL_TRIANGLES, model.IndexCount(), GL_UNSIGNED_SHORT, firstIndex);
	GL_GET_ERROR();

	if(useVbo)
    {
		// Unbind buffers from global state.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		GL_GET_ERROR();
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//                          - Mesh Renderable -
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Make one
////////////////////////////////////////////////////////////////////////////////
MeshRenderable3D::MeshRenderable3D(Renderer3D&              renderer,
                                   const Entity3D&          transformable,
                                   const std::string&       meshFile,
                                   const std::string&       textureFile,
                                   const Color&             ambient) :
    Renderable3D(renderer),
    transformableRef(transformable),
    ambient(ambient)
{
    mesh    = gResourceManager.LoadModel3D(meshFile);
    texture = gResourceManager.LoadTexture(textureFile);
}

////////////////////////////////////////////////////////////////////////////////
// Delete one
////////////////////////////////////////////////////////////////////////////////
MeshRenderable3D::~MeshRenderable3D()
{
    gResourceManager.ReleaseResource(mesh);
    gResourceManager.ReleaseResource(texture);
}

////////////////////////////////////////////////////////////////////////////////
// Render - Just send the data to the rederer to render
////////////////////////////////////////////////////////////////////////////////
void MeshRenderable3D::Render(Renderer3D& renderer)
{
    renderer.Render(transformableRef.Transform(), *mesh, *texture, ambient);
}

////////////////////////////////////////////////////////////////////////////////
// Render - Just send the data to the rederer to render
////////////////////////////////////////////////////////////////////////////////
void MeshRenderable3D::Render(RenderManager3D& renderManager)
{
//    renderer.Render(transformableRef.Transform(), *mesh, *texture, ambient);
}


////////////////////////////////////////////////////////////////////////////////
// SetTexture
////////////////////////////////////////////////////////////////////////////////
void MeshRenderable3D::SetTexture(Texture *t)
{
    gResourceManager.ReleaseResource(texture);
    texture = t;
}
 
*/



