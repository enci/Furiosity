////////////////////////////////////////////////////////////////////////////////
//  VertexFormats.h
//
//  Created by Bojan Endrovski on 19/05/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef VERTEXFORMATS_H
#define VERTEXFORMATS_H

// Local includes
#include "Frmath.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"

namespace Furiosity
{
    // /Used by the GUI and some other elemnts
    struct VertexPosition2DTexture
    {
        Vector2 Position;           // 8 Bytes
        Vector2 Texture;            // 8 Bytes
        // Thightly packed, no padding
    };
    
    
    /// This is the format OBJ uses that
    struct VertexPositionNormalTexture
    {
        Vector3 Position;       // 16 bytes
        Vector3 Normal;         // 16 bytes
        Vector2 Texture;        // 8 bytes
        //
        //char padding[8];        // Explicit padding 8 bytes
    };
    
    /// Used for SVG images. The color idicates a gradient.
    struct VertexPosition2DColor
    {
        Vector2 position;
        Color   color;
    };
    
    
    /// This is the format DebugDraw3D uses
    struct VertexPosition3DColor
    {
        Vector3 Position;           // 16 bytes
        Furiosity::Color	Color;  // 4 bytes
    };
}

#endif
