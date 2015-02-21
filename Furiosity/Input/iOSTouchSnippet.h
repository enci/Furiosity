////////////////////////////////////////////////////////////////////////////////
//  Snippet.impl
// Used as a header so that is can be included places
//
//  Created by Bojan Endrovski on 12/29/12.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// A never changable, yet always present part of every project
// this snippet call all the right stuff with zero safety
////////////////////////////////////////////////////////////////////////////////


//#define LOG_TOUCH_EVENTS

// iOS 7 hax-fix
- (BOOL)prefersStatusBarHidden
{
    return YES;
}


- (Touch)fromUITouch:(UITouch*)uitouch
{
    Touch t;
    
    // This should like totally work
    t.ID = (long)uitouch;                                    // Cast pointer to int as ID
    t.Phase = (TouchPhase)uitouch.phase;                    // Check this!!
    
    // Location
    float x = [uitouch locationInView:uitouch.view].x;
    float y = [uitouch locationInView:uitouch.view].y;
    t.Location = Vector2(x, y);
    
    // Prev Location
    x = [uitouch previousLocationInView:uitouch.view].x;
    y = [uitouch previousLocationInView:uitouch.view].y;
    t.PreviousLocation = Vector2(x, y);
    
    // Taps
    t.TapCount = (int)uitouch.tapCount;
    //printf("Touch x=%i y=%i\n", (int)x, (int)y);
    
    // All done
    return t;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *t in touches)
    {
        Touch frtouch = [self fromUITouch:t];
#ifdef LOG_TOUCH_EVENTS
        NSLOG(@"IOS Touch %d began \n", frtouch.ID);
#endif
        gInputManager.AddTouch(frtouch);
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *t in touches)
    {
        Touch frtouch = [self fromUITouch:t];
        gInputManager.UpdateTouch(frtouch);
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *t in touches)
    {
        Touch frtouch = [self fromUITouch:t];
#ifdef LOG_TOUCH_EVENTS
        NSLOG(@"IOS Touch %d ended \n", frtouch.ID);
#endif
        gInputManager.RemoveTouch(frtouch);
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *t in touches)
    {
        Touch frtouch = [self fromUITouch:t];
#ifdef LOG_TOUCH_EVENTS
        NSLOG(@"IOS Touch %d canceled \n", frtouch.ID);
#endif
        gInputManager.RemoveTouch(frtouch);
    }
}