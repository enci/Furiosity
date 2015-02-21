////////////////////////////////////////////////////////////////////////////////
//  ValueSmoother.h
//
//  Created by Boyan Endrovski on 11/11/11.
//  Copyright (c) 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

using std::vector;

namespace Furiosity
{
    ///
    /// Can be used to smooth a certain value over time. Is works as a simple
    /// ring buffer, sampleling the last entries in it.
    ///
    template <class T>
    class ValueSmoother
    {
        vector<T>   history;
        
        int         nextUpdateSlot;
        
        int         sampleSize;
        
    public:
        /// Ctor - to instantiate a value smoother pass
        /// it the number of samples you want
        ValueSmoother(int sampleSize):
            history(sampleSize),
            sampleSize(sampleSize),
            nextUpdateSlot(0)
        {}

        
        /// Dtor
        ~ValueSmoother() {}
        
        /// Get the value of the smoother
        T Value() const
        {
            // Now to calculate the average of the history list
            T sum = history[0];
            for (int i = 1; i < history.size(); ++i)
                sum += history[i];
            
            return sum * (1 / (float)history.size());
        }
        
        /// Each time you want to get a new average, feed it the most recent value
        /// and this method will return an average over the last sampleSize updates
        T Update(const T& last)
        {
            if( history.size() < sampleSize )
                history.push_back(last);
            else
                history[nextUpdateSlot++] = last;   // Overwrite the oldest value with the newest
        
            // Make sure m_nextUpdateSlot wraps around. 
            if (nextUpdateSlot == history.size())
                nextUpdateSlot = 0;
            
            return Value();
        }
        
        void Clear()
        {
            history.clear();
        }
    };
}