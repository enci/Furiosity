////////////////////////////////////////////////////////////////////////////////
// Device.cpp
//
// Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Device.h"

using namespace Furiosity;
using namespace std;


#ifdef IOS

#include <sys/types.h>
#include <sys/sysctl.h>


NSString* Platform()
{
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = (char*)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString *platform = [NSString stringWithUTF8String:machine];
    free(machine);
    return platform;
}

Device::HardwareDeviceType Device::GetHardwareDevice()
{
    NSString* platform = Platform();
    
    HardwareDeviceType deviceType = UNKNOWN;
    
    if ([platform isEqualToString:@"iPhone1,1"])    return IPHONE_1G;
    if ([platform isEqualToString:@"iPhone1,2"])    return IPHONE_3G;
    if ([platform isEqualToString:@"iPhone2,1"])    return IPHONE_3GS;
    if ([platform isEqualToString:@"iPhone3,1"])    return IPHONE_4;
    if ([platform isEqualToString:@"iPhone3,2"])    return IPHONE_4;
    if ([platform isEqualToString:@"iPhone3,3"])    return IPHONE_4_VERIZON;
    if ([platform isEqualToString:@"iPhone4,1"])    return IPHONE_4S;
    if ([platform isEqualToString:@"iPhone5,1"])    return IPHONE_5;
    if ([platform isEqualToString:@"iPhone5,2"])    return IPHONE_5;
    if ([platform isEqualToString:@"iPod1,1"])      return IPOD_1G;
    if ([platform isEqualToString:@"iPod2,1"])      return IPOD_2G;
    if ([platform isEqualToString:@"iPod3,1"])      return IPOD_3G;
    if ([platform isEqualToString:@"iPod4,1"])      return IPOD_4G;
    if ([platform isEqualToString:@"iPod5,1"])      return IPOD_5G;
    if ([platform isEqualToString:@"iPad1,1"])      return IPAD;
    if ([platform isEqualToString:@"iPad2,1"])      return IPAD_2_WIFI;
    if ([platform isEqualToString:@"iPad2,2"])      return IPAD_2_GSM;
    if ([platform isEqualToString:@"iPad2,3"])      return IPAD_2_CDMA;
    if ([platform isEqualToString:@"iPad2,4"])      return IPAD_2;
    if ([platform isEqualToString:@"iPad2,5"])      return IPAD_MINI;
    if ([platform isEqualToString:@"iPad3,1"])      return IPAD_3G_WIFI;
    if ([platform isEqualToString:@"iPad3,2"])      return IPAD_3G_4G;
    if ([platform isEqualToString:@"iPad3,3"])      return IPAD_3G_4G;
    if ([platform isEqualToString:@"iPad4,1"])      return IPAD_4G;
    if ([platform isEqualToString:@"i386"])         return SIMULATOR;
    if ([platform isEqualToString:@"x86_64"])       return SIMULATOR;
    
    return deviceType;
}

string Device::GetHardwareString()
{
    NSString* platform = Platform();
    const char* cstr = [platform cStringUsingEncoding:NSUTF8StringEncoding];
    return string(cstr);
}

string Device::GetDeviceID()
{
    const char* cID     = NULL;
    //
    if( SYSTEM_VERSION_LESS_THAN(@"6.0") )
    {
        cID = "99a9ac10-c6ff-11e2-8b8b-0800200c9a66";
        /*
         NOT ALOWED ANYMORE
        cID = [[[UIDevice currentDevice] uniqueIdentifier] UTF8String];
        */
    }
    else
    {
        NSUUID* uuid = [[UIDevice currentDevice] identifierForVendor];
        cID = [[uuid UUIDString] UTF8String];
    }
    //
    string id(cID);
    return id;
}

#endif



Vector2 Device::PhysicalScreenSize() const
{
#ifdef IOS
    
    HardwareDeviceType type = GetHardwareDevice();
    
    switch (type)
    {            
        case IPHONE_1G:
        case IPHONE_3G:
        case IPHONE_3GS:
        case IPHONE_4:
        case IPHONE_4_VERIZON:
        case IPHONE_4S:
        case IPOD_1G:
        case IPOD_2G:
        case IPOD_3G:
        case IPOD_4G:
            return Vector2(49.36f, 74.05f);
            
        case IPHONE_5:
        case IPOD_5G:
            return Vector2(49.36f, 87.15f);
            
        case IPAD:
        case IPAD_2_WIFI:
        case IPAD_2_GSM:
        case IPAD_2_CDMA:
        case IPAD_2:
        case IPAD_3G_WIFI:
        case IPAD_3G_4G:
        case IPAD_4G:
            return Vector2(150.36f, 200.48f);
            
        case IPAD_MINI:
            return Vector2(120.36f, 160.48f);            
            
        default:
            // Not yet released device
            if( [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone )
                return Vector2(49.36f, 87.15f);
            else
                return Vector2(150.36f, 200.48f);
    }
    
    
#elif ANDRIOD
    return Vector2(0, 0);
#endif
}





// end