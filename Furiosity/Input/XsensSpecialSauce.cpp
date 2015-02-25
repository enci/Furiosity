////////////////////////////////////////////////////////////////////////////////
//  XsensSpecialSauce.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 09/12/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "XsensSpecialSauce.h"

#ifdef ANDROID

#include "Logging.h"
#include "Input.h"

using namespace Furiosity;

XsensSpecialSauce::XsensSpecialSauce(InputManager& inputManager) :
    inputManager(inputManager)
{
    // Use xkf3ce
    xkf3ceFitler = xkf3ce_create_filter(
                                        52.2405, 	// lat
                                        6.8291,		// lon
                                        0,			// alt
                                        2014,		// year
                                        12,			// month
                                        25			// day
                                        );
    LOG("\n\n\n xkf3ce filter created! \n\n\n\n");
}

void XsensSpecialSauce::UpdateSensors(float values[], uint64_t timestamp, int type)
{
    if(xkf3ceFitler == nullptr)
        return;
    
    // LOG("xkf3ce type= %d timestamp=%lld vals= %f  %f  %f ", type, timestamp, values[0], values[1], values[2]);
    
    xkf3ce_input_data data;
    data.type = type;
    data.timestamp = timestamp;
    memcpy(data.data, values, 3*sizeof(float));
    int res = xkf3ce_one_filter_step(xkf3ceFitler, &data);
    
    if(res != 0)
    {
        // Use date
        xkf3ce_results results;
        xkf3ce_get_results(xkf3ceFitler, &results);
        
        // Orientation
        float quat[4];
        memcpy(quat, results.gaming.q_ls, 4*sizeof(float));
        //LOG("xkf3ce quat= %f  %f  %f  %f", quat[0], quat[1], quat[2], quat[3]);
        Matrix44 orientation = Matrix44::CreateFromQuaternion(quat);
        inputManager.UpdateOrientation(orientation);
        
        // Gravity
        float up[3];
        memcpy(up, results.up, 3*sizeof(float));
        //UpdateGravity(-9.8f * up[0], -9.8f * up[1], -9.8f * up[2]);
        inputManager.UpdateGravity(-up[0], -up[1], -up[2]);
        //UpdateGravity(0, 0, -9.8f);
    }
}

#endif

