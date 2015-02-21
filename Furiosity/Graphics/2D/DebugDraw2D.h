////////////////////////////////////////////////////////////////////////////////
//  DebugDraw2D.h
//
//  Created by Bojan Endrovski on 18/03/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef DEBUG

// Framework includes
#include "gl.h"

// Local includes
#include "Frmath.h"
#include "Color.h"
#include "Camera2D.h"
#include "Shader.h"

namespace Furiosity
{
    ///
    /// Debug drawing in 2d space (good for testing ideas and stuff). Once a drawing
    /// max has been reached, all subsequent draw calls are silently ignored.
    ///
    class DebugDraw2D 
    {
    protected:
        static int const maxLines = 1024;
        static const int maxPoints = 1024;

        
        // Lines
        int     linesCount;
        Vector2 vertexArray[maxLines * 2];
        Color   vertexColors[maxLines * 2];
        
        // Points
		int     pointsCount;
		Vector2 pointArray[maxPoints];
		Color   pointColors[maxPoints];
                
        // A pointer to the camera
        const Camera2D* camera;

        // Use a unique pointer to delete this Shader at the end of execution
        // and raw pointers to non-owning params an
        unique_ptr<Effect> shader;
        ShaderAttribute* attribColor        = nullptr;
        ShaderAttribute* attribVertex       = nullptr;
        ShaderParameter* paramCamera        = nullptr;

    public:
        // No args, for static creation
        DebugDraw2D();
        
        /// Does nothing
        virtual ~DebugDraw2D();
        
        /// Call to make debug draw, gets the shader program id
        /// All is to be handeled by the Renderer
        void Initialize(const Camera2D& camera);
        
        /// Used to  set a diffrent view
        void SetCamera(const Camera2D& camera) { this->camera = &camera; }
         
        /// Call when done
        void ShutDown();
        
        /// Queues a line for drawing
		void AddLine(const Vector2& from,
                     const Vector2& to,
                     const Color& color = Furiosity::Color::Red);
				
		/// Queues a line for drawing
		void AddCircle(const Vector2& center,
                       float radius         = 5.0f,
                       const Color& color   = Furiosity::Color::Red,
                       int divs             = 12);
		
		/// Queues a point for drawing 
		void AddPoint(const Vector2& position, const Color& color = Furiosity::Color::Red);
        
        /// Queues a rectangle for drawing 
		void AddRectangle(const Vector2& min,
                          const Vector2& max,
                          const Color& color = Furiosity::Color::Red);
        
        /// Get the camera being used 
        const Camera2D& Camera() const { return *camera; }
        
        /// Clears queued actions
		void Clear();
		
		/// Draw queue
		void Draw();
    };
    
    /// Manager global
    extern DebugDraw2D gDebugDraw2D;
}

#endif  // DEBUG