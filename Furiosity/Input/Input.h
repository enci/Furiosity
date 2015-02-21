////////////////////////////////////////////////////////////////////////////////
//  Input.h
//
//  Created by Bojan Endrovski on 23/05/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_H
#define INPUT_H

// Framework includes
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <deque>
#include <cstdint>

// Local includes
#include "Frmath.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix33.h"
#include "ValueSmoother.h"
#include "Defines.h"

#ifdef IOS
	#import <CoreMotion/CoreMotion.h>
#elif ANDROID
	#include "XsensSpecialSauce.h"
#endif


namespace Furiosity
{
    class Camera2D;
    
    class InputManager;
    
    // Don't ever change the index or ordering of these items. New items
    // can be added to the rear, though.
    enum TouchPhase
    {
        TOUCH_PHASE_BEGAN       = 0,
        TOUCH_PHASE_MOVED       = 1,
        TOUCH_PHASE_STATIONARY  = 2,
        TOUCH_PHASE_ENDED       = 3,
        TOUCH_PHASE_CANCELLED   = 4,
        TOUCH_PHASE_INVALID     = 5
    };
    
    ///
    /// Basic touch structure. Made to mimic the UITouch of iOS.
    ///
    struct Touch
    {
        friend class InputManager;
        
        // ID is equivalent to the API pointer
        long            ID;
        
        // Phase
        TouchPhase      Phase;
        
        // Location in view
        Vector2         Location;
        
        // Location on the previous refresh, not update!
        Vector2         PreviousLocation;
        
        // Tap count, unsed so far
        int             TapCount;
        
    private:
        // A flag whether this touch has been handled
        bool            handled;
        
        // A debug help to see who is getting this touch
        void*           handledBy;

    public:
        /// Handling wrapper
        bool            Handled() const { return handled; }
        void            Handle(void* handler);
        void            Unhandle();
        
        const void*     HandledBy() const { return handledBy; }

        std::string toString() const {
            std::stringstream ss;
            ss << "Touch[" << ID << "]: x:" << Location.x << " y:" << Location.y << " - ";

            switch(Phase) {
            case TOUCH_PHASE_BEGAN:
                return ss.str() + "TOUCH_PHASE_BEGAN";
            case TOUCH_PHASE_MOVED:
                return ss.str() + "TOUCH_PHASE_MOVED";
            case TOUCH_PHASE_STATIONARY:
                return ss.str() + "TOUCH_PHASE_STATIONARY";
            case TOUCH_PHASE_ENDED:
                return ss.str() + "TOUCH_PHASE_ENDED";
            case TOUCH_PHASE_CANCELLED:
                return ss.str() + "TOUCH_PHASE_CANCELLED";
            case TOUCH_PHASE_INVALID:
                return ss.str() + "TOUCH_PHASE_INVALID";
            default:
                return ss.str() + "UNKNOWN TOUCH PHASE!";
            }
        }
    };
    
    // Inteface for handling touch
    class TouchHandler
    {
    protected:
        float layer;
        
    public:
        TouchHandler(bool autosubscribe = true, float layer = 0.0f);
        virtual ~TouchHandler();
        
        virtual void HandleTouch(Touch& touch)  = 0;
        virtual void DropTouches()              = 0;
        float   Layer() const  { return layer;  }
        void SetLayer(float l) { layer = l;     }
        
        bool operator<(const TouchHandler &other) const
        { return layer < other.layer; }
    };
    
    // A shortcut interface for anythig that wants to be clickable
    class Clickable : public TouchHandler
    {
    protected:
        // Used to define the position of the object in space
        const Matrix33&     local;
        
        // Radius to react to
        float               radius;
        
        // Used to unproject the touch
        const Camera2D&     camera;
            
        // Current touch
        Touch*              touch;
        
        // This means that clicking will be done on tap, not on release
        bool                tappable;
        
    public:
        // Ctor
        Clickable(const Matrix33& transform,
                  float radius,
                  const Camera2D& camera,
                  bool tappable         = false,
                  bool autosubscribe    = true);
        
        // Dtor
        virtual ~Clickable();
        
        // From TouchHandler
        virtual void HandleTouch(Touch& touch);
        virtual void DropTouches();
        
        // Implement this one.
        virtual void Clicked() = 0;
    };
    
    ///
    /// InputManager provides and fusion of inputs available accross the engine
    /// through a static manager
    ///
    class InputManager
    {
    private:
        std::map<long, Touch>       touches;
        
        std::vector<long>           removeQueue;
        
        std::vector<TouchHandler*>  subscribers;
        
        Matrix44                    attitude;
        
        Matrix44                    attitudeMult;
        
        Vector3                     gravity;
        
        Vector3                     acceleration;
        
#ifdef IOS
        // Gyro info
        CMMotionManager*    motionManager;
        
        CMAttitude*         currentAttitude;
        
        CMAttitude*         referenceAttitude;

#elif ANDROID
        XsensSpecialSauce*  specialSauce;
#endif
        
    public:
        /// Do nothing ctor, static allocation only
        /// Object invalid util intialized!
        InputManager() {}
        
        /// The actcing constructor, must be called at the start of the game
        void Initialize();

        // TODO: Add shutdown and remove  xkf3ceFitler
        // OnPause and OnResume should respond and do the same
                
        /// Adds a new touch to the touch tracking
        /// and returns the actual touch kept in the engine
        Touch& AddTouch(const Touch& touch);
        
        /// Updates the position and other data of a tracked touch
        /// and returns the actual touch kept in the engine
        Touch& UpdateTouch(const Touch& touch);
        
        /// Remove touch tracking and return the actual in-engine touch
        Touch& RemoveTouch(const Touch& touch);

        /// Return true is touch is still valid touch and on screen
        bool IsTouchValid(const Touch& touch);
        
        /// Number of active touches
        int NumTouches() const;
        
        /// Update internls of this manager
        void Update(float dt);
        
        /// Return the collection of touches
        std::map<long, Touch>& Touches() { return touches; }
        
        void Subscribe(TouchHandler* handler);
        
        void Unsubscribe(TouchHandler* handler);
        
#if DEBUG
        void DebugRender(const Camera2D& camera);
#endif
        
        ////////////////////////////////////////////////////////////////////////////////
        // Motions sensing stuff
        ////////////////////////////////////////////////////////////////////////////////
        
        /// Enables the motion sensors. Device's response is not instant.
        bool EnableMotionSensors();
        
        /// The current orientation of the device
        const Matrix44& Orientation() const         { return attitude; }
        
        const Vector3& Accelerometer() const        { return acceleration; }
        
        const Vector3& Gravity() const              { return gravity; }
        
        void ResetOrientaton();
        
        void MultiplyOrientation(const Matrix44& orientation) { attitudeMult = orientation; }
        
#ifdef ANDROID
        /// Generic call for update of the motion sensors. Not documented due to signed NDAs
        void UpdateSensors(float values[], uint64_t timestamp, int type);
#endif

#ifdef IOS
        // On IOS this should not be visible to the outside
        // On Android these need to ba called from the Java side
    protected:
#endif
        void UpdateAccelerometer(double x, double y, double z);
        
        void UpdateGravity(double x, double y, double z);
        
        void UpdateOrientation(const Matrix44& att) { attitude = attitudeMult * att; }        

    protected:
        void CallSubcribers(Touch& touch);
    };
    
    // Global forward declaration
    extern InputManager gInputManager;
}

#endif
