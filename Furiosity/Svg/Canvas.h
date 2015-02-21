////////////////////////////////////////////////////////////////////////////////
//  Canvas.h
//
//  Created by Gerard Meier on 7/19/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//
// Useful link: http://www.songho.ca/opengl/gl_vbo.html
//

#pragma once

#include "gl.h"
#include "VertexFormats.h"
#include <vector>

namespace Furiosity
{
    ///
    /// No in use, not stable - remove untill done
    ///
    class Canvas
    {
        /// Needs access to VBOs and such:
        friend class CanvasRenderer;
    
    protected:
        /// Pointers to OpenGL vertex buffer objects.
        /// First index contains the Position / Color structs
        /// Second index contains the drawing indices.
        GLuint vbo[2];

        /// Temp collection to indicate the GL_TRIANGLE_FAN vertices:
        ///std::vector<Internal::Shape> shapes;
        
        /// RAM buffers prior to compiling/uploading.
        std::vector<unsigned short> indices;
        std::vector<VertexPosition2DColor> vertices;
        
        /// Prior to calculating gradient colors, vertices are stored
        /// here.
        std::vector<Vector2> buffer;
        
        /// Apply gradient, and generate a vertex format that the shader expects:
        void GenerateVertices(std::vector<Vector2>& buffer, const Color& a, const Color& b, const float& angle);

    public:
        Canvas();
        virtual ~Canvas();
    
        /// Calculate gradient and upload the vertices to the GPU. After compiling
        /// this canvas instance can be rendered through a CanvasRenderer instance.
        void Compile();
    
        /// Start a new polygon
        Canvas& Begin();
        
        /// Add a Vertex to the current polygon:
        Canvas& Vertex(const Vector2& p);
        Canvas& Vertex(const float& p1, const float& p2) { return Vertex(Vector2(p1, p2)); }
        
        /// Fill with a solid color:
        Canvas& Fill(const Color& color);
        
        /// Fill with a linear gradient:
        Canvas& Fill(const Color& a, const Color& b, const float& angle = 0);

        Canvas& Stroke(const float& border, const Color& a, const float& angle = 0);
        Canvas& Stroke(const float& border, const Color& colorBegin, const Color& colorEndconst, const float& angle = 0);

        /// Draw a nth order bezier curve.
        ///
        /// @param points std::vector containing all points. The first point is
        //                the start, last is the end. Control points are in the
        ///               middle.
        ///
        Canvas& Bezier(const std::vector<Vector2>& points);

        /// Mostly used for Android asset reloading.
        /// TODO: imlement and thoroughly test this.
        virtual void Reload();
        virtual bool IsValid();
        virtual void Invalidate();
    };

}