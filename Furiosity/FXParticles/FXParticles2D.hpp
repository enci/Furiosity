#include "GeneralManager.h"

namespace Furiosity
{


// Definition
template <class ParticleT>
GLint FXParticleManager2D<ParticleT>::uniforms[UNIFORM_COUNT];

////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
template <class ParticleT>
FXParticleManager2D<ParticleT>::FXParticleManager2D(ushort max,
                                                    const Camera2D* camera,
                                                    const string& texfile)
:   BaseFXParticleManager<ParticleT, Vector2>(max),
    camera(camera),
    attribColor(-1),
    attribPosition(-1),
    shader(0)
{
    // Load shaders and prepare for rendering
    LoadShaders();
    
    // Load texture
    texture = gResourceManager.LoadTexture(texfile, true);
}

////////////////////////////////////////////////////////////////////////////////
// Dtor
////////////////////////////////////////////////////////////////////////////////
template <class T>
FXParticleManager2D<T>::~FXParticleManager2D()
{
    gResourceManager.ReleaseResource(texture);

    if(shader != 0) {
        shader->RemoveReloadEvent(this);
        gResourceManager.ReleaseResource(shader);
    }
}

////////////////////////////////////////////////////////////////////////////////
// LoadShader
////////////////////////////////////////////////////////////////////////////////
template <class T>
bool FXParticleManager2D<T>::LoadShaders()
{
    shader = Furiosity::gResourceManager.LoadShader(
        "/SharedResources/Shaders/Particles2D.vsh",
        "/SharedResources/Shaders/Particles2D.fsh");
    // shader->AddReloadEvent(this, &FXParticleManager2D::ShadersReloaded);

    GLint program = shader->GetProgram();

    // Get attribute locations:
    attribColor    = glGetAttribLocation(program, "a_color");
    attribPosition = glGetAttribLocation(program, "a_position");
    GL_GET_ERROR();
    
    // Get uniform locations.
    uniforms[FXParticleManager2D::UNIFORM_ZOOM]    = glGetUniformLocation(program, "u_zoom");
    uniforms[FXParticleManager2D::UNIFORM_TEXSAMPLER] = glGetUniformLocation(program, "s_texture");
    uniforms[FXParticleManager2D::UNIFORM_PROJECTION] = glGetUniformLocation(program, "u_projection");
    GL_GET_ERROR();

    return true;    
}

////////////////////////////////////////////////////////////////////////////////
// Render
////////////////////////////////////////////////////////////////////////////////
template <class T>
void FXParticleManager2D<T>::Render()
{
    GLint program = shader->GetProgram();
    glUseProgram(program);
        
    glEnable (GL_BLEND);    
    //glBlendEquation(GL_MAX_EXT);
//    glBlendEquationSeparateOES( GL_ADD, GL_MAX_EXT);
    //glBlendFunc (GL_ZERO, GL_ONE);    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    
    //glBlendFunc (GL_ZERO, GL_ONE);
    GL_GET_ERROR();
    
    
    // Update uniform value projection
    Matrix33 proj = camera->Projection();
    glUniformMatrix3fv(uniforms[UNIFORM_PROJECTION],    // Location
                       1,                               // Count
                       0,                               // Transpose (row vs column major)
                       &proj.m11);                      // Value
    GL_GET_ERROR();
    
    //glUniform1f(uniforms[UNIFORM_ZOOM], camera->Zoom() * this->pixelScaling);
    glUniform1f(uniforms[UNIFORM_ZOOM],  (gGeneralManager.ScreenWidth() * this->pixelScaling) / camera->Window().x);
    
    
    // Bind texture
    glActiveTexture(GL_TEXTURE0);                   // Use texture 0
    glBindTexture(GL_TEXTURE_2D, texture->name);    // Work with this texture
    glUniform1i(uniforms[UNIFORM_TEXSAMPLER], 0);   // Set the sampler to tex 0
    GL_GET_ERROR();

    
    // Update attribute position values
    // !!! Also the size hides in the Z !!!
    glVertexAttribPointer(attribPosition,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(T),
                          &this->particles[0].Position);
    glEnableVertexAttribArray(attribPosition);
    GL_GET_ERROR();
    
    // Update attribute color values
    glVertexAttribPointer(attribColor,
                          4,
                          GL_UNSIGNED_BYTE,
                          GL_TRUE,
                          sizeof(T),
                          &this->particles[0].CurrentColor);
    glEnableVertexAttribArray(attribColor);
    GL_GET_ERROR();
    
    glDrawArrays(GL_POINTS, 0, this->maxParticles);
    GL_GET_ERROR();
}
    
}
