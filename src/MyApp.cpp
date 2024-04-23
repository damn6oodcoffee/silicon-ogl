
#include "MyApp.h"


wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
	wxInitAllImageHandlers();
	MyFrame* frame{ new MyFrame("Silicon S-W", wxDefaultPosition, wxDefaultSize) };
	frame->Show(true);
	return true;
}