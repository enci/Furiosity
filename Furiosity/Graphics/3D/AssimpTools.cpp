////////////////////////////////////////////////////////////////////////////////
//  AssimpTools.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 11/01/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AssimpTools.h"

using namespace Furiosity;

Color AssimpTools::ConvertColor(const aiColor3D& color)
{
    return Color((uchar)(color.r * 255.0f),
                 (uchar)(color.g * 255.0f),
                 (uchar)(color.b * 255.0f),
                 (uchar)(255));
}

Matrix44 AssimpTools::ConvertMatrix(const aiMatrix4x4& m)
{
    return Matrix44 (m.a1, m.b1, m.c1, m.d1,
                     m.a2, m.b2, m.c2, m.d2,
                     m.a3, m.b3, m.c3, m.d3,
                     m.a4, m.b4, m.c4, m.d4);
}


