#pragma once
#include <wx/timer.h>
#include "OGLPane.h"
#include <mutex>
#include <thread>

class RenderTimer : public wxTimer
{
public:
    OGLPane* pane;
    mutex* mtx;
public:
    bool bver = false;
    RenderTimer(OGLPane* pane);
    void Notify();
    void start();
};

