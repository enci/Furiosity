////////////////////////////////////////////////////////////////////////////////
//  Vector3.cpp
//
//  Created by Bojan Endrovski on 20/11/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Vector3.h"
#include "Frmath.h"

using namespace Furiosity;

float Vector3::AngleTo(const Vector3 &rhs)
{
    float dot = Dot(rhs);
    float lm = Magnitude();
    float rm = rhs.Magnitude();
    //
    float val = acosf( dot / (lm * rm) );
    return val;
}

Vector3 Vector3::RandomInsideUnitSphere()
{
    Vector3 v;
    do
    {
        v.x = RandInRange(-1.0f, 1.0f);
        v.y = RandInRange(-1.0f, 1.0f);
        v.z = RandInRange(-1.0f, 1.0f);
    }
    while (v.Magnitude() > 1.0f);
    //
    return v;
}

Vector3 Vector3::RandomOnUnitSphere()
{
    Vector3 v = RandomInsideUnitSphere();
    v.Normalize();
    return v;
}

Vector3 Vector3::RandBetween(const Vector3& from, const Vector3& to)
{
    return Vector3(RandInRange(from.x, to.x),
                   RandInRange(from.y, to.y),
                   RandInRange(from.z, to.z));
}

Vector3 Vector3::Infinity(FLT_MAX, FLT_MAX, FLT_MAX);