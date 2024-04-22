#include "RenderTimer.h"


RenderTimer::RenderTimer(OGLPane* pane) : wxTimer()
{
    RenderTimer::pane = pane;
}

void RenderTimer::Notify()
{
    while (!mtx->try_lock()) {};
    pane->Refresh();
    pane->Update();
    mtx->unlock();
    
    //if (bver) pane->silicon->Verlet();
}

void RenderTimer::start()
{
    wxTimer::Start(1e3 * 1.0/30);
 
}