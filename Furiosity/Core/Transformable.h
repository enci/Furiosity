////////////////////////////////////////////////////////////////////////////////
//  Transformable.h
//
//  Created by Bojan Endrovski on 26/02/2013.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Matrix33.h"

namespace Furiosity
{
    class Transformable
    {
    protected:
        Matrix33& _trns;
        
        Vector2 scale;
        
        float   rotation;
    
    public:
        /// Ctor th
        Transformable(Matrix33& transformableMatrix) :
            _trns(transformableMatrix) {}
        
        
        ~Transformable() {}
        
        const Matrix33& Transform() const { return _trns; }
        
        void SetTransform(const Matrix33& trns)    { _trns = trns; }

        // Position
        Vector2 Position() const        { return _trns.Translation(); }
        void SetPosition(Vector2 pos)   { _trns.SetTranslation(pos);  }
        void Translate(Vector2 move)    { _trns.Translate(move);      }
        
        // Rotation
        void SetRotation(float r)       { _trns.SetRotation(r);    }
        
        float Rotation() const          { return _trns.Rotation(); }
        
        // void SetScale(float s)         { _trns.SetScale(Vector2(s, s)); }
        // void SetScale(Vector2 s)       { _trns.SetScale(s);             }
        // Vector2 Scale() const          { return _trns.Scaling();        }
    };
}
