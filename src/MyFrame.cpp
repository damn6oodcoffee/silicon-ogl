#include "MyFrame.h"
#include <fstream>

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size) {

	
	
	auto rightPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(300,600));

	mtx = new mutex();
	

	int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
	oglPane = new OGLPane(this, args);
	oglPane->silicon = new Silicon();
	oglPane->silicon->Init(3);

	//ChartPanel* chartPanel = new ChartPanel(rightPanel, wxID_ANY);
	wxButton* rndPos = new wxButton(rightPanel, wxID_ANY, "Случайный\nсдвиг");
	wxButton* rndVel = new wxButton(rightPanel, wxID_ANY, "Задать\nскорость");
	wxButton* reset = new wxButton(rightPanel, wxID_ANY, "Сброс");
	wxButton* delAtom = new wxButton(rightPanel, wxID_ANY, "Удалить\nцентральный\nатом");
	step = new wxButton(rightPanel, wxID_ANY, "Старт");
	wxCheckBox* rlxCheck = new wxCheckBox(rightPanel, wxID_ANY, "Релаксация");
	rlxCheck->Bind(wxEVT_CHECKBOX, &MyFrame::OnChecked, this);
	rndPos->Bind(wxEVT_BUTTON, &MyFrame:: OnRandPos, this);
	rndVel->Bind(wxEVT_BUTTON, &MyFrame::OnRandVel, this);
	reset->Bind(wxEVT_BUTTON, &MyFrame::OnReset, this);
	delAtom->Bind(wxEVT_BUTTON, &MyFrame::OnDeleteAtom, this);
	step->Bind(wxEVT_BUTTON, &MyFrame::OnStep, this);
	auto rightSizer = new wxBoxSizer(wxVERTICAL);
	rightSizer->Add(rndPos, 0);
	rightSizer->Add(rndVel, 0);
	rightSizer->Add(reset, 0);
	rightSizer->Add(delAtom, 0);
	rightSizer->Add(step, 0);
	rightSizer->Add(rlxCheck, 0);
	//rightSizer->Add(chartPanel, 1, wxEXPAND);
	rightSizer->AddStretchSpacer(2);


	auto mainSizer = new  wxBoxSizer(wxHORIZONTAL);


	mainSizer->Add(oglPane,5, wxALL | wxEXPAND, FromDIP(10));

	mainSizer->Add(rightPanel, 0, wxALL | wxEXPAND, FromDIP(10));
	
	rightPanel->SetSizerAndFit(rightSizer);
	this->SetSizerAndFit(mainSizer);
	this->SetSize(wxSize(800, 600));

	timer = new RenderTimer(oglPane);
	timer->start();

	timer->mtx = mtx;
	oglPane->silicon->mtx = mtx;

	ofstream myfile("energy.txt");
	myfile.close();

	ofstream myfile2("temp.txt");
	myfile2.close();
}

void MyFrame::OnRandPos(wxCommandEvent &event) {
	oglPane->silicon->RandomAtomsShift(true, false);
}

void MyFrame::OnRandVel(wxCommandEvent& event) {
	oglPane->silicon->RandomAtomsShift(false, true);
}

void MyFrame::OnReset(wxCommandEvent& event) {
	oglPane->silicon->Reset();

	ofstream myfile("energy.txt");
	myfile.close();

	ofstream myfile2("temp.txt");
	myfile2.close();
}

void MyFrame::OnStep(wxCommandEvent& event) {

	if (!t1) {
		bCalcInProc = true;
		t1 = new thread(&MyFrame::CalcVerlet, std::ref(*this));
		step->SetLabelText("Стоп");
	}
	else {
		bCalcInProc = false;
		t1->join();
		delete t1;
		t1 = nullptr;
		step->SetLabelText("Старт");
	}
	/*
	oglPane->silicon->Verlet();
	timer->bver = !timer->bver;
	*/
}

void MyFrame::CalcVerlet() {
	while (bCalcInProc) oglPane->silicon->Verlet();
}

void MyFrame::OnDeleteAtom(wxCommandEvent& event) {
	oglPane->silicon->DeleteCentralAtom();
}


void MyFrame::OnChecked(wxCommandEvent& event) {
	bRlx = !bRlx;
	oglPane->silicon->bRlx = bRlx;
	
}