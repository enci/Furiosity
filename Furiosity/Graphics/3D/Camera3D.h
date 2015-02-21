////////////////////////////////////////////////////////////////////////////////
//  Camera3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/13/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Matrix44.h"
#include "Input.h"
#include "Entity3D.h"
#include "RenderLayarable3D.h"

namespace Furiosity
{
    ///
    /// Camera3D
    ///
    class Camera3D : public Entity3D, public RenderLayarable3D
    {
    protected:
        
        /// Flag wheter the view matrix should be re-calculated from the
        /// world transform
        bool dirty = true;
        
        /// Cached view matrix
        Matrix44 view;
        
        /// Projection matrix
        Matrix44 projection;
        
        /// Updates the view matrix a caches the result
        void UpdateViewMatrix();
        
    public:
        
        /// Create a new camera
        Camera3D(World3D* world3D, Entity3D* parent) : Entity3D(world3D, parent, -1.0f) {}
        
        /// Update the camera
        virtual void Update(float dt) override;
        
        /// Make this camera active. This will deactivate other cameras shared by the same
        /// world, but will not affect cameras not attached to the same world.
        void SetActive();
        
        /// Get the view matrix
        const Matrix44& View()                  { UpdateViewMatrix(); return view; }
        
        /// Get the projection matrix
        const Matrix44& Projection() const      { return projection; }
        
        /// Get the view projection matrix of this camera
        Matrix44 ViewProjection()               { return projection * View(); }
        
        /// Set the projection matrix
        void SetProjection(const Matrix44& p)   { projection = p; }
        
        /// Set the view matrix. This will affect the trasform of the Camera
        void SetView(const Matrix44& v);
        
        // const Matrix44& ViewProjection()
        
        Vector2 Project(const Vector3& v);
        
        /// Unproject canonical-space vector
        Vector3 Unproject(const Vector3& v);
        
        /// Unproject canonical-space vector to a ray
        Ray3D Unproject(const Vector2& v);

        /// Unproject screen space vector to a ray
        Ray3D UnprojectScreen(const Vector2& screenCoor);
    };
    
    
    /*
    class SimpleCamera3D : public Camera3D
    {
    public:
        SimpleCamera3D(World3D* world3D, Entity3D* parent) : Camera3D(world3D, parent) {}
        
        //void SetView(const Matrix44& v)        { wor view = v; }
        
        void SetProjection(const Matrix44& p)  { projection = p; }
    };
    
    class ArcCamera : public Camera3D, public TouchHandler
    {
    protected:
        float inclination;
        float azimuth;        
        float distance;
        
        Touch*  ptouch;
        
    public:
        
        ArcCamera(World3D* world3D, Entity3D* parent, float distance);
        
        void SetProjection(const Matrix44& p)  { projection = p; }
        
        void Update();
        
        /// @see TouchHandler
        virtual void HandleTouch(Touch& touch);
        
        /// @see TouchHandler
        virtual void DropTouches();

    };
     
    */
}



// end
