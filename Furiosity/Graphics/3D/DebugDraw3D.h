////////////////////////////////////////////////////////////////////////////////
//  DebugDraw3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/4/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Framework includes
#include "gl.h"

// Local includes
#include "Frmath.h"
#include "Color.h"
#include "Shader.h"
#include "Matrix44.h"
#include "Camera3D.h"
#include "VertexFormats.h"


namespace Furiosity
{
    ///
    /// Debug drawing in 3d space (good for testing ideas and stuff). Once a drawing
    /// max has been reached, all subsequent draw calls are silently ignored.
    ///
    class DebugDraw3D
    {
    protected:
        // Static arrays
        static int const maxLines = 4096;
        static const int maxPoints = 1024;
        
        // Lines
        int                     linesCount;
        VertexPosition3DColor   vertexArray[maxLines * 2];

        // Points
		int                     pointsCount;
		VertexPosition3DColor   pointArray[maxPoints];
        
        // Use a unique pointer to delete this Shader at the end of execution
        // and raw pointers to non-owning params an
        unique_ptr<Effect> shader;
        ShaderAttribute* attribColor        = nullptr;
        ShaderAttribute* attribVertex       = nullptr;
        ShaderParameter* paramCamera        = nullptr;
        
        // A non-owning pointer to the camera
        Camera3D* camera                    = nullptr;
        
    public:
        // No args, for static creation
        DebugDraw3D();
        
        /// Does nothing
        virtual ~DebugDraw3D();
        
        /// Call to make debug draw, gets the shader program id
        /// All is to be handeled by the Renderer
        void Initialize(Camera3D& camera);
        
        /// Used to  set a diffrent view.
        /// Call this when creating a new world (order;])
        void SetCamera(Camera3D& camera) { this->camera = &camera; }
                
        /// Queues a line segment for drawing
		void AddLine(const Vector3& from,
                     const Vector3& to,
                     const Color& color = Furiosity::Color::Red);
        
		/// Queues a circle for drawing
		void AddCircle(const Vector3& center,
                       float radius         = 1.0f,
                       const Color& color   = Furiosity::Color::Red,
                       int divs             = 12);
        
        /// Queues a sphere for drawing
		void AddAxisSphere(const Vector3& center,
                           float radius         = 1.0f,
                           const Color& xzColor = Furiosity::Color::Green,
                           const Color& xyColor = Furiosity::Color::Blue,
                           const Color& yzColor = Furiosity::Color::Red,
                           int divs             = 12);

        /// Queues a sphere for drawing
		void AddSphere(const Vector3& center,
                       float radius         = 1.0f,
                       const Color& color   = Furiosity::Color::Red,
                       int divs             = 12)
        { AddAxisSphere(center, radius, color, color, color,divs); }
        
        /// Queues an axis display
        void AddAxis(const Matrix44& trans, float size);
		
		/// Queues a point for drawing
		void AddPoint(const Vector3& position, const Color& color = Furiosity::Color::Red);
        
        /// Clears queued actions
		void Clear();
		
		/// Draw queue
		void Draw();
    };
    
    /// Manager global
    extern DebugDraw3D gDebugDraw3D;
}
