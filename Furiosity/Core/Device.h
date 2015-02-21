////////////////////////////////////////////////////////////////////////////////
// Device.h
//
// Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "Vector2.h"
#include "Defines.h"

#ifdef IOS
#   import <UIKit/UIKit.h>
#endif

namespace Furiosity
{
    
/**
* Class Device.
*/
class Device
{
public:
    
#ifdef IOS
    
    enum HardwareDeviceType
    {
        IPHONE_1G,
        IPHONE_3G,
        IPHONE_3GS,
        IPHONE_4,
        IPHONE_4_VERIZON,
        IPHONE_4S,
        IPHONE_5,
        IPOD_1G,
        IPOD_2G,
        IPOD_3G,
        IPOD_4G,
        IPOD_5G,
        IPAD,
        IPAD_2_WIFI,
        IPAD_2_GSM,
        IPAD_2_CDMA,
        IPAD_2,
        IPAD_MINI,
        IPAD_3G_WIFI,
        IPAD_3G_4G,
        IPAD_4G,
        SIMULATOR,
        UNKNOWN
    };
        
    static HardwareDeviceType   GetHardwareDevice();

    static std::string          GetHardwareString();
    
    static std::string          GetDeviceID();
    
    #define SYSTEM_VERSION_EQUAL_TO(v)                  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
    #define SYSTEM_VERSION_GREATER_THAN(v)              ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
    #define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
    #define SYSTEM_VERSION_LESS_THAN(v)                 ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
    #define SYSTEM_VERSION_LESS_THAN_OR_EQUAL_TO(v)     ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)
    
#endif

    
    
private:
    float screenWidth;
    float screenHeight;

    float resolutionWidth;
    float resolutionHeight;
    
    float pixelScale;
    
    std::string deviceID;

public:
    ///
    /// Default constructor for static instantiation.
    ///
    Device() = default;

    ///
    /// Default copy constructor.
    ///
    Device(const Device&) = default;

    ///
    /// A constructor for constructing.
    ///
    Device(float screenWidth,
           float screenHeight,
           float pixelScale,
           const std::string& deviceID) :
        screenWidth(screenWidth),
        screenHeight(screenHeight),
        pixelScale(pixelScale),
        deviceID(deviceID)
    {
        this->resolutionWidth  = screenWidth * pixelScale;
        this->resolutionHeight = screenHeight * pixelScale;
    }

    ///
    /// Retrieve the virtual screen width. The OS will scale this when rendered.
    ///
    /// @returns virtual screen width
    ///
    float GetScreenWidth() const { return screenWidth; };

    ///
    /// Retrieve the virtual screen height. The OS will scale this when rendered.
    ///
    /// @returns virtual screen height
    ///
    float GetScreenHeight() const { return screenHeight; };

    ///
    /// Retrieve the physical screen width. This figure will match the real
    /// pixel count.
    ///
    /// @returns physical screen width
    ///
    float GetResolutionWidth() const { return resolutionWidth; };

    ///
    /// Retrieve the physical screen height. This figure will match the real
    /// pixel count.
    ///
    /// @returns physical screen height
    ///
    float GetResolutionHeight() const { return resolutionHeight; };
    
    
    float GetPixelScale() const { return pixelScale; }
    
    
    const std::string& DeviceID() const { return deviceID; }
    
    /// PhysicalScreenSize
    ///
    /// @returns The physical screen size in centimeters
    ///
    Vector2 PhysicalScreenSize() const;
};

}





// end
