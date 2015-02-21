////////////////////////////////////////////////////////////////////////////////
//  Camera3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/13/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Camera3D.h"
#include "GeneralManager.h"
#include "Frmath.h"
#include "DebugDraw3D.h"
#include "World3D.h"

using namespace Furiosity;

void Camera3D::UpdateViewMatrix()
{
    //   if(dirty)
    //{
        view = Matrix44(worldTrns);
        view.Invert();
        dirty = false;
    //}
}

void Camera3D::Update(float dt)
{
    Entity3D::Update(dt);
    UpdateViewMatrix();
}

void Camera3D::SetActive()
{
    world3D->SetActiveCamera(this);
}

void Camera3D::SetView(const Matrix44 &v)
{
    Matrix44 m = v;         // Save to a matrix
    m.Invert();             // Invert that
    SetTransformation(m);   // And use it as transformation
    view = v;               // The value for view we have
    dirty = false;          // so, no need to recalculate
}

Vector2 Camera3D::Project(const Vector3& v)
{
    Vector4 vec(v.x, v.y, v.z);
    Matrix44 vp = ViewProjection();
    Vector4 proj = vp * vec;
    
    return Vector2(proj.x / proj.z, proj.y / proj.z);
}

Vector3 Camera3D::Unproject(const Furiosity::Vector3 &v)
{
    Vector4 vec(v.x, v.y, v.z);
    Matrix44 vpinv = ViewProjection();
    vpinv.Invert();
    
    Vector4 unproj = vpinv * vec;
    unproj.NormalizeW();
    
    return Vector3(unproj.x, unproj.y, unproj.z);
}

Ray3D Camera3D::Unproject(const Vector2 &v)
{
    // Unproject a vector at the near plane
    Vector3 from = Unproject( Vector3(v.x, v.y, 0.0f) );
    // Unproject a vector at the far plane
    Vector3 to =   Unproject( Vector3(v.x, v.y, 1.0f) );
        
    // Make a ray out of the two vectors
    Ray3D ray;
    ray.Origin = from;
    ray.Direction = to - from;
    ray.Direction.Normalize();
    return ray;
}

Ray3D Camera3D::UnprojectScreen(const Furiosity::Vector2 &screenCoor)
{
    // Bring to [-1, 1] range
    float w = gGeneralManager.ScreenWidth();
    float h = gGeneralManager.ScreenHeight();
    
    // Unproject canonical space vector
    Vector2 canonical((2*screenCoor.x - w) / w, (h - 2*screenCoor.y) / h);
    
    // Log some data if needed for debugging
    //LOG("Touch Screen:[%4.1f:%4.1f] to canonical [%2.3f:%2.3f]\n", screenCoor.x, screenCoor.y, canonical.x, canonical.y);
    
    // Unproject the canonical using the method for that
    return Unproject(canonical);
}


/*
ArcCamera::ArcCamera(World3D* world3D, Entity3D* parent, float distance) :
    Camera3D(world3D, parent),
    distance(distance),
    azimuth(0.0f),
    inclination(HalfPi),
    ptouch(0)
{
    
}

void ArcCamera::Update()
{
    Vector3 eye;
    SphericalToCartesian(eye.x, eye.y, eye.z,
                         distance, inclination, azimuth);
    
    view = Matrix44::CreateLookAt(eye, Vector3(), Vector3(0, 1, 0));
}

void ArcCamera::HandleTouch(Touch &touch)
{
    if(ptouch == 0) // No touch
    {
        
        if((touch.Phase == TOUCH_PHASE_BEGAN ||
            touch.Phase == TOUCH_PHASE_MOVED ||
            touch.Phase == TOUCH_PHASE_STATIONARY) &&
            touch.Handled() == false)
        {
            ptouch = &touch;
            ptouch->Handle(this);
        }
    }
    else
    {
        // See of this is our touch
        if(&touch == ptouch)
        {
            // Check if it's going out of fashion
            if(touch.Phase == TOUCH_PHASE_ENDED     ||
               touch.Phase == TOUCH_PHASE_CANCELLED ||
               touch.Phase == TOUCH_PHASE_INVALID)
            {
                ptouch = 0;
            }
            else
            {
                Vector2 move = touch.Location - touch.PreviousLocation;
                azimuth += move.x * 0.003f;
                inclination -= move.y * 0.003f;
            }
        }
    }
}

void ArcCamera::DropTouches()
{
    ptouch = 0;
}
*/


// end