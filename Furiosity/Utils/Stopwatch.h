////////////////////////////////////////////////////////////////////////////////
//  Stopwatch.h
//
//  Created by Bojan Endrovski on 5/20/13.
//  Copyright (c) 2013 Bojan Endrovski1. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Furiosity
{
    ///
    /// Does exactly what you'd expect.
    ///
    class Stopwatch
    {
    public:
        /// Construct a new Stopwatch object for measuring time. The stopwatch
        /// is not automatically started.
        ///
        /// @see Start()
        Stopwatch();
        
        /// Starts the stopwatch. Any previous measurement is reset.
        void Start();
        
        /// Record the current time elapsed, then pause recording. A paused
        /// Stopwatch can be resumed later to accumulate more measurements.
        ///
        /// @returns The time elapsed thus far.
        double Pause();
        
        /// Continues a paused stopwatch. Time elapsed between pause and
        /// continue is not recorded.
        ///
        /// @see Pause()
        void Continue();
        
        /// Stop measuring time and return the elapsed time. A stopped
        /// stopwatch shouldn't be continued.
        ///
        /// @returns The timing elapsed thus far.
        double Stop();
        
        /// Peek at the current elapsed time. Does not modify the stopwatch.
        ///
        /// @returns The time elapsed thus far.
        double Elapsed() const;
        
    private:
        double elapsed;
        double startTime;
    };
}
