////////////////////////////////////////////////////////////////////////////////
//  Camera2D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 06/04/2012.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef CAMERA2D_H
#define CAMERA2D_H

#include "Vector2.h"
#include "Matrix33.h"
#include "Input.h"
#include <cassert>

namespace Furiosity
{
    class Camera2D : public TouchHandler
    {
        enum Camera2DState
        {
            CameraStateNone,
            CameraStateLimbo,
            CameraStatePanning,
            CameraStateZooming,
            CameraStatePanZoom,
        };
        
    public:
        enum Camera2DControlFlags
        {
            CAMERA2D_ENABLE_NONE    = 0,
            CAMERA2D_ENABLE_PAN     = 1,
            CAMERA2D_ENABLE_ZOOM    = 2
        };
        
    protected:
        
        // A 2D projection like matrix
        Matrix33 projection;
        
        // An inverse of the above
        Matrix33 invproj;
        
        // Window
        float windowx;
        float windowy;
        
        // Center
        float centerx;
        float centery;
        //
        float panFromX;
        float panFromY;        
        
        // Zoom
        float zoom;
        float zoomDefault;
        float zoomMax;
        float zoomMin;
        
        // Bounds in which  it can move
        float maxx;
        float minx;
        float maxy;
        float miny;
        
        // The state of this camera
        Camera2DState state;
        
        // Handle to two touches for paning and zooming
        Touch*  touch0;
        Touch*  touch1;
        
        // Keeps the control flag for the camera
        int flags;
        
    public:
        
        /// Creates a new camera
        ///
        /// @param wx Window size along the x-axis
        /// @param wy Window size along the y-axis
        /// @param cx Centar's x-axis
        /// @param wy Centar's y-axis
        /// @param fitScreen Wheter to fit given aspect ratio to screen
        /// @param flags A bitmap of camera controls to enable
        Camera2D(float wx, float wy,
                 float cx, float cy,
                 bool fitScreen = true,
                 int flags = CAMERA2D_ENABLE_NONE);
        
        /// Dtor
        virtual ~Camera2D();
        
        
        virtual void Update(float dt);
        
        /// \brief The projection matrix used for rendering (return a copy)
        const Matrix33& Projection() const         { return projection; }
        
        // Window into the world that this camera is currently using
        // TODO: Fix this with fit to window and sutff
        Vector2 Window() const { return Vector2(windowx, windowy); }
        void    SetWindow(Vector2 window);
        void    SetWindow(float x, float y);
        
        // Camera center
        Vector2 Center() const              { return Vector2(centerx, centery); }
        void    SetCenter(float x, float y)
        {
            centerx = x;
            centery = y;
        }
        //
        void    SetCenter(Vector2 center)   { centerx = center.x; centery = center.y; }
        
        // Zoom commands
        // TODO: Fix this to change the window too
        float   Zoom()      const { return zoom; }
        float   ZoomMin()   const { return zoomMin; }
        float   ZoomMax()   const { return zoomMax; }
        void    SetZoom(float zoom, float min, float max);
        void    SetZoom(float zoom);
        
        // Pan
        // TODO: This might be better protected
        void    PanWithWindowCoor(float x, float y);
        void    BeginPan(float x, float y);
        
        // Small utils
        Vector2 Unproject(const Vector2& vec) const;
        Vector2 Project(const Vector2& vec) const;
        
        // This can limit the camera's movement
        void    SetBounds(float xmin, float ymin, float xmax, float ymax);
        
        /// @see TouchHandler
        virtual void HandleTouch(Touch& touch);
        
        /// @see TouchHandler
        virtual void DropTouches();

    };
}





#endif
