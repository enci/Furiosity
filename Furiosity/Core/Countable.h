////////////////////////////////////////////////////////////////////////////////
//  Countable.h
//
//  Created by Bojan Endrovski on 3/26/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Defines.h"
#include <map>

//#define KEEP_REGISTER


namespace Furiosity
{
    // A countable faclity, based on a answer from
    // http://stackoverflow.com/questions/5827903/can-template-classes-have-static-methods-in-c
    //
    template <typename T>
    class Countable
    {
    private:
        /// Declaration of data member, definition must appear later,
        /// even if there is no initializer.
        static uint active;
        
    public:
        
#ifdef KEEP_REGISTER
        /// Keep all the objects in a single place for inspection
        static std::map< long, Countable<T>* > centralRegister;
#endif
        
    protected:
        /// Create contable, adding to active count
        Countable()
        {
            active++;
#ifdef KEEP_REGISTER
            centralRegister[(long)this] = this;
#endif
        }
        
        /// Delete contable, removing from active count
        ~Countable()
        {
            active--;
#ifdef KEEP_REGISTER
            centralRegister.erase((long)this);
#endif
        }
        
    public:
        
        /// Active
        ///
        /// @return the number of active instance of this (templated) class
        static uint Active() { return active; }
    };
    
    // The only way to templatize a (non-function) object is to make it a static
    // data member of a class. This declaration takes the form of a template yet
    // defines a global variable, which is a bit special.
    template <typename T>
    uint Countable<T>::active = 0;
    
#ifdef KEEP_REGISTER
    /// Keep all the objects in a single place for inspection
    template <typename T>
    std::map< long, Countable<T>* > Countable<T>::centralRegister;
#endif
}
