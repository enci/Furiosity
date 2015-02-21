////////////////////////////////////////////////////////////////////////////////
//  Stopwatch.cpp
//
//  Created by Bojan Endrovski on 5/20/13.
//  Copyright (c) 2013 Bojan Endrovski1. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Stopwatch.h"
#include "Utils.h"

using namespace Furiosity;

Stopwatch::Stopwatch() : elapsed(0), startTime(0)
{
}

void Stopwatch::Start()
{
    elapsed = 0;
    startTime = Furiosity::GetTiming();
}


void Stopwatch::Continue()
{
    startTime = Furiosity::GetTiming();
}


double Stopwatch::Pause()
{
    double now = Furiosity::GetTiming();
    elapsed += now - startTime;
    
    return elapsed;
}


double Stopwatch::Stop()
{
    Pause();
    startTime = 0;
    return elapsed;
}

double Stopwatch::Elapsed() const
{
    double now = Furiosity::GetTiming();
    return elapsed + (now - startTime);
}