////////////////////////////////////////////////////////////////////////////////
//  Camera2D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 06/04/2012.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Camera2D.h"
#include "DebugDraw2D.h"
#include "GeneralManager.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
Camera2D::Camera2D(float wx, float wy, float cx, float cy, bool fit, int flags) :
    windowx(wx),
    windowy(wy),
    centerx(cx),
    centery(cy),
    maxx(FLT_MAX),
    minx(-FLT_MAX),
    maxy(FLT_MAX),
    miny(-FLT_MAX),
    state(CameraStateNone),
    touch0(0),
    touch1(0),
    flags(flags),
    // Only handle touches if needed
    TouchHandler( flags != CAMERA2D_ENABLE_NONE )
{
    projection.SetIdentity();
    invproj.SetIdentity();
    
    if(fit)
    {
        // Screen
        float Ws = gGeneralManager.ScreenWidth();
        float Hs = gGeneralManager.ScreenHeight();
        float Rs = Ws / Hs;
        
        // World
        float Ww = wx;
        float Hw = wy;
        float Rw = Ww / Hw;
        
        // Init
        windowx = Ww;
        windowy = Hw;
        
        // Set to fit
        if(Rs > Rw)
            windowx = windowy * Rs;
        else
            windowy = windowx * (1.0f / Rs);
    }
    
    // Create matrices from params
    SetZoom(1, 1, 1);
    Update(0.0f);
}


////////////////////////////////////////////////////////////////////////////////
// Dtor - does nothing
////////////////////////////////////////////////////////////////////////////////
Camera2D::~Camera2D() {}


////////////////////////////////////////////////////////////////////////////////
// Update
////////////////////////////////////////////////////////////////////////////////
void Camera2D::Update(float dt)
{    
    // Set scaling factors
    float scalex = (zoom * 2.0f) / windowx;
    float scaley = (zoom * 2.0f) / windowy;
    float framex = windowx / (zoom * 2.0f);
    float framey = windowy / (zoom * 2.0f);
    
    /*
    //
    gDebugDraw2D.AddLine(Vector2( centerx - framex, centery - framey ),
                         Vector2( centerx + framex, centery + framey ),
                         Color::Orange);
    gDebugDraw2D.AddCircle(Vector2( centerx - framex, centery - framey ),
                           1.5f,
                           Color::Orange);
    gDebugDraw2D.AddCircle(Vector2( centerx + framex, centery + framey ),
                           1.5f,
                           Color::Orange);
    */
    
    // Check bounds
    float overx     = maxx - (centerx + framex);
    float overy     = maxy - (centery + framey);
    //
    float underx    = minx - (centerx  - framex);
    float undery    = miny - (centery  - framey);
    //
    if(overx < 0)
        centerx += overx;
    else if(underx > 0)
        centerx += underx;
    //
    if(overy < 0)
        centery += overy;
    else if(undery > 0)
        centery += undery;

    
    Matrix33 view;
    view.SetIdentity();
    view.SetTranslation(Vector2(centerx, centery));
    view = view.Inverse();
    
    Matrix33 proj;
    proj.SetIdentity();
    proj.SetScale(Vector2(scalex, scaley));
    
    projection = proj;
    projection.Multiply(view);
    
    invproj = projection.Inverse();    
}

////////////////////////////////////////////////////////////////////////////////
// Handle touch
// This gets called only if the camera has controls enabled
////////////////////////////////////////////////////////////////////////////////
void Camera2D::HandleTouch(Touch &touch)
{
    switch (state)
    {
        case CameraStateNone:
        {
            if( (touch.Phase == TOUCH_PHASE_BEGAN ||
                 touch.Phase == TOUCH_PHASE_MOVED ||
                 touch.Phase == TOUCH_PHASE_STATIONARY) &&
                 touch.Handled() == false)
            {
                touch0 = &touch;
                touch0->Handle(this);
                state = CameraStatePanZoom;
                BeginPan(touch0->Location.x, touch0->Location.y);
            }
            
            break;
        }
            
        case CameraStatePanZoom:
        {
            Vector2 center;
            
            // Panning only
            if(touch1 == 0)
            {
                // See of this is our touch
                if(&touch == touch0)
                {
                    // Check if it's going out of fashion
                    if(touch.Phase == TOUCH_PHASE_ENDED     ||
                       touch.Phase == TOUCH_PHASE_CANCELLED ||
                       touch.Phase == TOUCH_PHASE_INVALID)
                    {
                        touch0 = 0;
                        state = CameraStateNone;
                    }
                    else
                    {
                        center = touch0->Location;
                        PanWithWindowCoor(center.x, center.y);
                    }
                }
                // Or if it's a new touch
                else if((touch.Phase == TOUCH_PHASE_BEGAN ||
                         touch.Phase == TOUCH_PHASE_MOVED ||
                         touch.Phase == TOUCH_PHASE_STATIONARY) &&
                         touch.Handled() == false)
                {
                    // We shall use this new touch for stuffs
                    touch1 = &touch;
                    touch1->Handle(this);

                    //state = CameraStatePanZoom;
                    //
                    center = (touch1->Location + touch0->Location) * 0.5f;
                    BeginPan(center.x, center.y);
                }
            }
            // Panning and zooming
            else
            {
                // Check if it's going out of fashion
                if(touch.Phase == TOUCH_PHASE_ENDED     ||
                   touch.Phase == TOUCH_PHASE_CANCELLED ||
                   touch.Phase == TOUCH_PHASE_INVALID)
                {
                    // Check if this is one of our thouches
                    //
                    if(&touch == touch0)
                    {
                        touch0 = touch1;
                        //
                        center = touch0->Location;
                        BeginPan(center.x, center.y);
                        //
                        touch1 = 0;
                    }
                    else if(&touch == touch1)
                    {
                        touch1 = 0;
                        //
                        center = touch0->Location;
                        BeginPan(center.x, center.y);
                    }
                }
                
                if(&touch == touch0 || &touch == touch1)
                {
                    center = touch0->Location;
                    if(touch1)
                    {
                        center += touch1->Location;
                        center *= 0.5f;
                        PanWithWindowCoor(center.x, center.y);
                        
                        // TODO: Zoom here
                        float prevdist = (touch0->PreviousLocation - touch1->PreviousLocation).Magnitude();
                        float currdist = (touch0->Location - touch1->Location).Magnitude();
                        //
                        float factor = powf(currdist / prevdist, 0.5f);
                        float z = Zoom();
                        z *= factor;
                        SetZoom(z);
                    }
                }

            }
                        
            break;
        }
                        
        default:
            assert(false);
    }
}

void Camera2D::DropTouches()
{
    state = CameraStateNone;
    touch0 = 0;
    touch1 = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Window
////////////////////////////////////////////////////////////////////////////////
void Camera2D::SetWindow(float x, float y)
{
    windowx = x;
    windowy = y;
    SetZoom(1.0f, 0.5f, 1.5f);
}

void Camera2D::SetWindow(Furiosity::Vector2 window)
{ SetWindow(window.x, window.y); }


////////////////////////////////////////////////////////////////////////////////
// Zoom
////////////////////////////////////////////////////////////////////////////////
void Camera2D::SetZoom(float zoom, float min, float max)
{
    this->zoom  = zoom;
    zoomDefault = zoom;
    zoomMin     = min;
    zoomMax     = max;
}

void Camera2D::SetZoom(float z)
{
    if(z > zoomMax)
        this->zoom = zoomMax;
    else if(z < zoomMin)
        this->zoom = zoomMin;
    else
        this->zoom = z;
    
    zoomDefault = this->zoom;
}


////////////////////////////////////////////////////////////////////////////////
// Pan
////////////////////////////////////////////////////////////////////////////////
void Camera2D::PanWithWindowCoor(float x, float y)
{
    // Pan
    float dx = x - panFromX;
    float dy = y - panFromY;
    //
    panFromX = x;
    panFromY = y;
    //
    centerx -= dx / zoom;
    centery += dy / zoom;    
}

void Camera2D::BeginPan(float x, float y)
{
    panFromX = x;
    panFromY = y;
    //
    PanWithWindowCoor(x, y);
}

////////////////////////////////////////////////////////////////////////////////
// Projection stuff
////////////////////////////////////////////////////////////////////////////////
Vector2 Camera2D::Unproject(const Vector2& vec) const 
{
    // Bring to [-1, 1] range
    float w = gGeneralManager.ScreenWidth();
    float h = gGeneralManager.ScreenHeight();
    //
    Vector2 res((2*vec.x - w) / w,
                (h - 2*vec.y) / h);
         
    // Transform it to world coordinates
    invproj.TransformVector2(res);
    return res;
}

////////////////////////////////////////////////////////////////////////////////
// Projection stuff
// TODO: For to work with
////////////////////////////////////////////////////////////////////////////////
Vector2 Camera2D::Project(const Vector2& vec) const
{
    Vector2 res = vec;
    
    projection.TransformVector2(res);
    
    // Bring from [-1, 1] range to screen resolution
    float w = gGeneralManager.ScreenWidth();
    float h = gGeneralManager.ScreenHeight();
    //
    res += Vector2(1.0f, 1.0f);
    res.x *= w * 0.5f;
    res.y = h - (res.y * h * 0.5f);
    
    // Return it
    return res;
}


////////////////////////////////////////////////////////////////////////////////
// Bounds
////////////////////////////////////////////////////////////////////////////////
void Camera2D::SetBounds(float xmin, float ymin, float xmax, float ymax)
{
    minx = xmin;
    maxx = xmax;
    miny = ymin;
    maxy = ymax;
}















// end
