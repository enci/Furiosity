////////////////////////////////////////////////////////////////////////////////
//  Vector2.cpp
//
//  Created by Bojan Endrovski on 23/09/2010.
//  Copyright 2010 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Frmath.h" // Gets its header form there

using namespace Furiosity;

// Vector from Xml DOM
Vector2::Vector2(const XMLElement* vec)
{
    assert(vec);
    
    // Get the strings
    const char* pX = vec->Attribute("x");
    const char* pY = vec->Attribute("y");
    // Check that all is fine
    assert(pX && pY);
    
    // Assign
    x = atof(pX);
    y = atof(pY);
}

Vector2 Vector2::RandomUnit()
{
    float rad = RandInRange(-Pi, Pi);
    //
    return Vector2( cosf(rad), sinf(rad));
}


Vector2 Vector2::Infinity(FLT_MAX, FLT_MAX);

Vector2 Vector2::Zero(0, 0);

/*
int Vector2:: Hash() const
{
    // Pick a weird coodrinate frame
    Vector2 origin(HalfPi, Pi);
    //
    union { float f; int i; } converter;
    
    converter.f = x;
    int xc = converter.i;
    //
    converter.f = y;
    int yc = converter.i;
    //
    converter.f = Distance(origin);
    int dc = converter.i;
    //
    return xc ^ yc ^ dc;
}
*/



// end
