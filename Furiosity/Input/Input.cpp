////////////////////////////////////////////////////////////////////////////////
//  Input.cpp
//
//  Created by Bojan Endrovski on 23/05/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Input.h"
#include "Defines.h"

#include "Camera2D.h"
#include "DebugDraw2D.h"
#include "GeneralManager.h"

using namespace Furiosity;

using namespace std;


////////////////////////////////////////////////////////////////////////////////
//
//                              - Touch -
//
////////////////////////////////////////////////////////////////////////////////
void Touch::Handle(void *handler)
{
    handled = true;
    
    handledBy = handler;
}

void Touch::Unhandle()
{
    handled = false;
    
    handledBy = 0;
}



////////////////////////////////////////////////////////////////////////////////
//
//                          - Touch Handler -
//
////////////////////////////////////////////////////////////////////////////////
TouchHandler::TouchHandler(bool autosubscribe, float layer) : layer(layer)
{
    if(autosubscribe)
        gInputManager.Subscribe(this);
}

TouchHandler::~TouchHandler()
{
    // Always safe to unsubscribe
    gInputManager.Unsubscribe(this);
}


////////////////////////////////////////////////////////////////////////////////
// Clickable
////////////////////////////////////////////////////////////////////////////////
Clickable::Clickable(const Matrix33& transform,
                     float radius,
                     const Camera2D& camera,
                     bool tappable,
                     bool autosubscribe) :
    TouchHandler(autosubscribe),
    local(transform),
    radius(radius),
    camera(camera),
    tappable(tappable),
    touch(0)
{}


Clickable::~Clickable() {}


// TODO: !!!This is plain defective!!!
void Clickable::HandleTouch(Touch& touch)
{
    Vector2 loc = camera.Unproject(touch.Location);
    float dist = (loc - local.Translation()).Magnitude();
    
    // A simple tap based click
    if (tappable && dist <= radius &&
        touch.Phase == TOUCH_PHASE_BEGAN &&
        !touch.Handled())
    {
        Clicked();
        return;
    }
    
    // Has a touch, waiting for it to end to begin a new click
    if (this->touch != 0)
    {
        if(this->touch == &touch)
        {
            if((dist > radius) ||
               touch.Phase == TOUCH_PHASE_CANCELLED    ||
               touch.Phase == TOUCH_PHASE_INVALID)
            {
                this->touch = 0;
            }
            else if(touch.Phase == TOUCH_PHASE_ENDED)
            {
                this->touch = 0;
                Clicked();
            }
        }
    }
    // Has no touch, so it's trying to find one
    else
    {
        if(touch.Handled())
            return;
#if DEBUG
        gDebugDraw2D.AddCircle(local.Translation(), radius, Color::White);
#endif
        
        if(dist <= radius &&
           (touch.Phase == TOUCH_PHASE_BEGAN ||
            touch.Phase == TOUCH_PHASE_STATIONARY) )
        {
            touch.Handle(this);
            this->touch = &touch;
        }
    }
}

void Clickable::DropTouches()
{
    touch = 0;
}



////////////////////////////////////////////////////////////////////////////////
//
//                          - Input Manager -
//
////////////////////////////////////////////////////////////////////////////////

// Manager definition
InputManager Furiosity::gInputManager;

void InputManager::Initialize()
{
#ifdef IOS
    motionManager   = nil;
    currentAttitude = nil;
    motionManager   = nil;
#elif ANDROID
    specialSauce    = nullptr;
#endif
    
    // Reset values
    gravity.Clear();
    acceleration.Clear();
    attitude.SetIdentity();
    attitudeMult.SetIdentity();
}

// Add a new touch to the map
Touch& InputManager::AddTouch(const Touch& touch)
{
    touches[touch.ID] = touch;
    touches[touch.ID].Unhandle();
    CallSubcribers(touches[touch.ID]);
    //
    return touches[touch.ID];
}

// Updates the information of an existing one
Touch& InputManager::UpdateTouch(const Touch& touch)
{
    bool handled = touches[touch.ID].Handled();
    void* handledBy = touches[touch.ID].handledBy;
    touches[touch.ID] = touch;
    touches[touch.ID].handled = handled;
    touches[touch.ID].handledBy = handledBy;
    CallSubcribers(touches[touch.ID]);
    //
    return touches[touch.ID];

}

// Prepares a touch for removing by setting some fields
Touch& InputManager::RemoveTouch(const Touch& touch)
{
    bool handled = touches[touch.ID].Handled();
    void* handledBy = touches[touch.ID].handledBy;
    touches[touch.ID] = touch;
    touches[touch.ID].handled = handled;
    touches[touch.ID].handledBy = handledBy;
    //touches[touch.ID].Phase = TOUCH_PHASE_ENDED;
    //
    removeQueue.push_back(touch.ID);
    CallSubcribers(touches[touch.ID]);
    //
    return touches[touch.ID];
}


// Prepares a touch for removing by setting some fields
bool InputManager::IsTouchValid(const Touch& touch)
{
    return touches.find(touch.ID) != touches.end();
}

int InputManager::NumTouches() const
{
    return (int)touches.size();
}

void InputManager::Update(float dt)
{
    // Remove stale touches
	for(long id : removeQueue)
        touches.erase(id);
    //
    removeQueue.clear();
    
    // Remove bad touches
	for (auto itr : touches)
	{
		Touch& touch = itr.second;
        Vector2& loc = touch.Location;
        
        if(isnan(loc.x) || isnan(loc.y) ||
           loc.x <= 0   || loc.x >= gGeneralManager.ScreenWidth() ||
           loc.y <= 0   || loc.y >= gGeneralManager.ScreenHeight())
        {
            touch.Phase = TOUCH_PHASE_INVALID;
            RemoveTouch(touch);
        }
    }
    
#ifdef IOS
    // Update motion sensing
    if(motionManager != nil)
    {
        // Get current atitude
        currentAttitude = motionManager.deviceMotion.attitude;
        
        // Check of device has a
        if(currentAttitude != nil)
        {
            if(referenceAttitude != nil)
                [currentAttitude multiplyByInverseOfAttitude: referenceAttitude];
            
            CMRotationMatrix rotation = currentAttitude.rotationMatrix;
            Matrix44 rotView(rotation.m11, rotation.m12, rotation.m13, 0.0f,
                             rotation.m21, rotation.m22, rotation.m23, 0.0f,
                             rotation.m31, rotation.m32, rotation.m33, 0.0f,
                             0.0f,         0.0f,         0.0f,         1.0f);
            UpdateOrientation(rotView);
        }

        CMAcceleration gr = motionManager.deviceMotion.gravity;
        UpdateGravity(gr.x, gr.y, gr.z);
        
        CMAcceleration accel = motionManager.accelerometerData.acceleration;
        UpdateAccelerometer(accel.x, accel.y, accel.z);
    }
#endif
}

void InputManager::UpdateAccelerometer(double x, double y, double z)
{
    acceleration = Vector3(x, y, z);
}

void InputManager::UpdateGravity(double x, double y, double z)
{
    gravity = Vector3(x, y, z);
}


#if ANDROID
void InputManager::UpdateSensors(float values[], uint64_t timestamp, int type)
{
    specialSauce->UpdateSensors(values, timestamp, type);
}
#endif


bool InputManager::EnableMotionSensors()
{
    bool flag = true;
    
#ifdef IOS
    motionManager = [[CMMotionManager alloc] init];  // Alcate and init sensor fusion
    motionManager.deviceMotionUpdateInterval = 0.01; // Set interval to 100 Hz
    
    // Enable gyro
    if (motionManager.isDeviceMotionAvailable)
        [motionManager startDeviceMotionUpdates];
    else
        flag = false;
    
    // Enable accerometer
    if (motionManager.isAccelerometerAvailable)
        [motionManager startAccelerometerUpdates];
    else
        flag = false;
    
#elif ANDROID
    specialSauce = new XsensSpecialSauce(*this);
#endif
    
    return flag;
}

void InputManager::ResetOrientaton()
{
#ifdef IOS
    referenceAttitude = [motionManager.deviceMotion.attitude retain];
#endif
}

void MultiplyOrientation(Matrix44& Orientation);

void InputManager::Subscribe(TouchHandler* handler)
{
    subscribers.push_back(handler);
}

void InputManager::Unsubscribe(TouchHandler* handler)
{
    vector<TouchHandler*>::iterator idx;
    for (idx = subscribers.begin(); idx != subscribers.end(); ++idx)
    {
        if (*idx == handler)
        {
            subscribers.erase(idx);
            break;
        }
    }
}

bool CompareLayers(TouchHandler* left, TouchHandler* right)
{
    return left->Layer() < right->Layer();
}

void InputManager::CallSubcribers(Touch &touch)
{
    // TODO: Stable sort?
    if(subscribers.size() > 1)
        std::sort(subscribers.begin(), subscribers.end(), CompareLayers);
    //
    for (auto subscriber : subscribers)
        subscriber->HandleTouch(touch);
}


#if DEBUG

void InputManager::DebugRender(const Camera2D& camera)
{
	// Remove bad touches
	for (auto itr : touches)
    {
		Touch touch = itr.second;
		touch = itr.second;
        Vector2 loc =  camera.Unproject(touch.Location);
        gDebugDraw2D.AddCircle(loc, 100, Color::Purple);
	}
}

#endif


