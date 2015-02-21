////////////////////////////////////////////////////////////////////////////////
//  Matrix33.cpp
//
//  Created by Bojan Endrovski on 18/03/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Matrix33.h"

using namespace Furiosity;

Matrix33::Matrix33(const XMLElement* settings)
{
    // Set to identity
    SetIdentity();
    
    // Set translation
    const XMLElement* translationEl = settings->FirstChildElement("translation");
    if(translationEl)
    {
        Vector2 translation(translationEl);
        SetTranslation(translation);
    }

    // Set rotation - in radians or degrees
    const XMLElement* rotationEl = settings->FirstChildElement("rotation");
    if(rotationEl)
    {
        float rotation = 0.0f;
        const char* pRad = rotationEl->Attribute("rad");
        if(pRad)
            rotation = atof(pRad);
        else
        {
            const char* pDeg = rotationEl->Attribute("deg");
            if(pDeg)
                rotation = atof(pDeg) * 0.0174532925f;
        }
                    
        SetRotation(rotation);
    }
}


Matrix33 Matrix33::Identity = Matrix33();
