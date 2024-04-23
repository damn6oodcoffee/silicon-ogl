#pragma once

#include "wx/wx.h"
#include "wx/glcanvas.h"
#include <gl/gl.h>
#include <gl/GLU.h>
#include "Silicon.h"

class OGLPane : public wxGLCanvas
{
	wxGLContext* m_context{ nullptr };
	bool bLeftDown{ false };
	bool bRightDown{ false };

	double zAngle{};
	double xyAngle{};
	double offsetX{};
	double offsetY{};
	wxPoint mouseClickPt{};

public:
	Silicon* silicon{ nullptr };

public:
	OGLPane(wxFrame* parent, int* args);
	virtual ~OGLPane();

	void resized(wxSizeEvent& evt);

	int getWidth();
	int getHeight();

	void render(wxPaintEvent& evt);
	void SetProjection();


	// events
	void mouseMoved(wxMouseEvent& event);
	void leftDown(wxMouseEvent& event);
	void leftUp(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void rightDown(wxMouseEvent& event);
	void rightUp(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	DECLARE_EVENT_TABLE()
};


void drawBox(double scale);