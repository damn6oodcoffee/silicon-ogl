#include "OGLPane.h"


BEGIN_EVENT_TABLE(OGLPane, wxGLCanvas)
EVT_MOTION(OGLPane::mouseMoved)
EVT_LEFT_DOWN(OGLPane::leftDown)
EVT_LEFT_UP(OGLPane::leftUp)
EVT_RIGHT_DOWN(OGLPane::rightDown)
EVT_RIGHT_UP(OGLPane::rightUp)
EVT_LEAVE_WINDOW(OGLPane::mouseLeftWindow)
EVT_SIZE(OGLPane::resized)
EVT_KEY_DOWN(OGLPane::keyPressed)
EVT_KEY_UP(OGLPane::keyReleased)
EVT_MOUSEWHEEL(OGLPane::mouseWheelMoved)
EVT_PAINT(OGLPane::render)
END_EVENT_TABLE()

OGLPane::OGLPane(wxFrame* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
    m_context = new wxGLContext(this);
 
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

}

OGLPane::~OGLPane()
{
    delete m_context;
}

int OGLPane::getWidth()
{
	return GetSize().x;
}

int OGLPane::getHeight()
{
	return GetSize().y;
}

void OGLPane::resized(wxSizeEvent& evt)
{
    //	wxGLCanvas::OnSize(evt);

    Refresh();
}

void OGLPane::SetProjection()
{
   

	glFrontFace(GL_CW);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//glOrtho(-2.0f, 2.0, -2.0f, 2.0f, -100.0f, 100.0f);
    gluPerspective(45.0f, (GLfloat)getWidth() /
       (GLfloat)getHeight(), 0.1f, 100.0f);


    glViewport(0, 0, getWidth(), getHeight());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OGLPane::render(wxPaintEvent& evt)
{

    if (!IsShown() && silicon) return;

    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
	
	glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    // ------------- draw some 3D ----------------
    SetProjection();
    glLoadIdentity();

	// Point the camera at the origin from the positive octant (x,y,z > 0).
	gluLookAt(5.0f, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);



	// Lighting 
	{
		GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };

		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		GLfloat white[] = { 0.8f, 0.8f, 0.8f, 0.0f };
		GLfloat cyan[] = { 1.0f, 0.7f, 0.0f, 0.f };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
		glMaterialfv(GL_FRONT, GL_SPECULAR, white);
		glMaterialf(GL_FRONT, GL_SHININESS, 128.0f);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	//

	glRotatef(xyAngle, -1.0f, 1.0f, 0.0f);
	glRotatef(zAngle, 0.0f, 0.0f, 1.0f);

	glTranslatef(offsetX, offsetY, 0);

	double atomPosScale = 1.5e9;
	for (auto& atom : silicon->atoms) {
		glPushMatrix();
		glTranslatef(atom.x * atomPosScale, atom.y * atomPosScale, atom.z * atomPosScale);
		gluSphere(gluNewQuadric(), 0.1, 16, 16);
		glPopMatrix();
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);


	/*
	for (int i = 0; i < silicon->atoms.size(); i++) {
		if (i == 0) {
			auto atom = silicon->atoms[i];
			for (auto& nb : atom.neighbours) {
				glBegin(GL_LINES);
				glColor3f(0.0f, 1.0f, 1.0f);
				glVertex3d(atom.x * 1.5e9, atom.y * 1.5e9, atom.z * 1.5e9);
				glVertex3d(nb->x * 1.5e9, nb->y * 1.5e9, nb->z * 1.5e9);
				glEnd();
			}
		}
	}
	*/

	glBegin(GL_LINES);
	glColor3f(0, 1, 0); glVertex3d(0.0f, 0.0f, 0.0f); glVertex3d(10.0f, 0.0f, 0.0f);	// x axis.
	glColor3f(0, 0, 1); glVertex3d(0.0f, 0.0f, 0.0f); glVertex3d(0.0f, 10.0f, 0.0f);	// y axis.
	glColor3f(1, 0, 0); glVertex3d(0.0f, 0.0f, 0.0f); glVertex3d(0.0f, 0.0f, 10.0f);	// z axis.
	glEnd();

	drawBox(silicon->xWall * atomPosScale);

    glFlush();
    SwapBuffers();
}



void OGLPane::mouseMoved(wxMouseEvent& event) {
	if (bLeftDown) {
		double scale = 2;
		if (event.GetPosition().x - mouseClickPt.x > 0) zAngle += 0.5*scale;
		else if (event.GetPosition().x - mouseClickPt.x < 0) zAngle -= 0.5*scale;
		if (event.GetPosition().y - mouseClickPt.y > 0) xyAngle += 0.5*scale;
		else if (event.GetPosition().y - mouseClickPt.y < 0) xyAngle -= 0.5*scale;

		if (abs(zAngle) > 360) zAngle = 0;
		if (abs(xyAngle) > 360) xyAngle = 0;
		mouseClickPt = event.GetPosition();
		//Refresh();
	}
	if (bRightDown) {
		double zAngleRad = -zAngle * M_PI / 180 + M_PI/4;
		double scale = 0.025;
		/*
		if (event.GetPosition().x - mouseClickPt.x > 0) offsetX += scale;
		else if (event.GetPosition().x - mouseClickPt.x < 0) offsetX -= scale;
		if (event.GetPosition().y - mouseClickPt.y > 0) offsetY += scale;
		else if (event.GetPosition().y - mouseClickPt.y < 0) offsetY -= scale;
		*/

		if (event.GetPosition().x - mouseClickPt.x > 0) { offsetY += scale * cos(zAngleRad); offsetX -= scale * sin(zAngleRad); }
		else if (event.GetPosition().x - mouseClickPt.x < 0) { offsetY -= scale * cos(zAngleRad); offsetX += scale * sin(zAngleRad); }
		if (event.GetPosition().y - mouseClickPt.y > 0) { offsetY += scale * sin(zAngleRad); offsetX += scale * cos(zAngleRad); }
		else if (event.GetPosition().y - mouseClickPt.y < 0) { offsetY -= scale * sin(zAngleRad); offsetX -= scale * cos(zAngleRad); }
		mouseClickPt = event.GetPosition();
		//Refresh();
	}
}

void OGLPane::leftDown(wxMouseEvent& event) {
	bLeftDown = true;
	mouseClickPt = event.GetPosition();
}

void OGLPane::leftUp(wxMouseEvent& event) {
	bLeftDown = false;
}

void OGLPane::mouseWheelMoved(wxMouseEvent& event) {

}

void OGLPane::rightDown(wxMouseEvent& event) {
	bRightDown = true;
}

void OGLPane::rightUp(wxMouseEvent& event) {
	bRightDown = false;
}

void OGLPane::mouseLeftWindow(wxMouseEvent& event) {
	bLeftDown = false;
	bRightDown = false;
}

void OGLPane::keyPressed(wxKeyEvent& event) {}

void OGLPane::keyReleased(wxKeyEvent& event) {}



void drawBox(double scale)
{
	glPushMatrix();
	glScalef(scale, scale, scale);


	glBegin(GL_LINES);



	glColor3f(0.0f, 0.0f, 0.0f);


	// TOP
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	// BOTTOM
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	// VERTICAL LINES
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);

	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glEnd();
	glPopMatrix();
}