#include "MyFrame.h"
#include "wx/gbsizer.h"
#include <wx/valnum.h>
#include <fstream>

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size) {

	auto rightPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(300,600));
	auto controlsSizer = new wxGridBagSizer();
	mtx = new mutex();
	
	int args[]{ WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
	int defaultCellSize{ 3 };

	oglPane = new OGLPane(this, args);
	oglPane->SetWindowStyleFlag(wxSUNKEN_BORDER);
	oglPane->silicon = new Silicon();
	oglPane->silicon->Init(defaultCellSize);

	//ChartPanel* chartPanel = new ChartPanel(rightPanel, wxID_ANY);
	wxIntegerValidator<int> sizeValidator{};
	sizeValidator.SetRange(1, 10);
	wxFloatingPointValidator<double> scaleValidator(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
	scaleValidator.SetRange(0.0, 100.0);
	wxFloatingPointValidator<double> coefValidator(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
	coefValidator.SetRange(0.0, 1.0);

	
	cellSize = new wxTextCtrl(rightPanel, wxID_ANY, wxString::Format(wxT("%i"), defaultCellSize));
	cellSize->SetValidator(sizeValidator);
	atomShift = new wxTextCtrl(rightPanel, wxID_ANY, "1");
	atomShift->SetValidator(scaleValidator);
	atomVel = new wxTextCtrl(rightPanel, wxID_ANY), "1";
	atomVel->SetValidator(scaleValidator);
	rlxCoef = new wxTextCtrl(rightPanel, wxID_ANY, "0.8");
	rlxCoef->SetValidator(coefValidator);

	applySize = new wxButton(rightPanel, wxID_ANY, "���������");
	rndPos = new wxButton(rightPanel, wxID_ANY, "���������\n�����");
	rndVel = new wxButton(rightPanel, wxID_ANY, "���������\n��������");
	reset = new wxButton(rightPanel, wxID_ANY, "�����");
	delAtom = new wxButton(rightPanel, wxID_ANY, "������� �����������\n����");
	step = new wxButton(rightPanel, wxID_ANY, "�����");
	rlxCheck = new wxCheckBox(rightPanel, wxID_ANY, "����������");
	
	applySize->Bind(wxEVT_BUTTON, &MyFrame::OnApplySize, this);
	rndPos->Bind(wxEVT_BUTTON, &MyFrame:: OnRandPos, this);
	rndVel->Bind(wxEVT_BUTTON, &MyFrame::OnRandVel, this);
	reset->Bind(wxEVT_BUTTON, &MyFrame::OnReset, this);
	delAtom->Bind(wxEVT_BUTTON, &MyFrame::OnDeleteAtom, this);
	step->Bind(wxEVT_BUTTON, &MyFrame::OnStep, this);
	rlxCheck->Bind(wxEVT_CHECKBOX, &MyFrame::OnChecked, this);

	/*
	auto rightSizer = new wxBoxSizer(wxVERTICAL);
	rightSizer->Add(rndPos, 0);
	rightSizer->Add(rndVel, 0);
	rightSizer->Add(reset, 0);
	rightSizer->Add(delAtom, 0);
	rightSizer->Add(step, 0);
	rightSizer->Add(rlxCheck, 0);
	//rightSizer->Add(chartPanel, 1, wxEXPAND);
	rightSizer->AddStretchSpacer(2);
	*/
	
	controlsSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "������ ��������� ������:"), { 0,0 }, { 1,1 }, wxALIGN_CENTER);
	controlsSizer->Add(cellSize, { 1,0 }, { 1,1 }, wxALIGN_CENTER);
	controlsSizer->Add(applySize, { 1,1 }, { 1,1 }, wxALIGN_CENTER | wxLEFT | wxRIGHT, FromDIP(5));
	controlsSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "�������� ������:"), { 2,0 }, { 1,1 }, wxALIGN_CENTER);
	controlsSizer->Add(atomShift, { 3,0 }, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(rndPos, { 3,1 }, {1,1}, wxALIGN_CENTER | wxLEFT | wxRIGHT, FromDIP(5));
	controlsSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "�������� ��������:"), { 4,0 }, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(atomVel, {5,0}, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(rndVel, { 5,1 }, {1,1}, wxALIGN_CENTER | wxLEFT | wxRIGHT, FromDIP(5));
	controlsSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "����. ����������:"), { 6,0 }, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(rlxCoef, { 7,0 }, {1,1},  wxALIGN_CENTER);
	controlsSizer->Add(rlxCheck, { 7,1 }, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(step, { 8,0 }, {1,1}, wxALIGN_CENTER | wxALL, FromDIP(5));
	controlsSizer->Add(reset, { 8, 1 }, {1,1}, wxALIGN_CENTER | wxALL, FromDIP(5));
	controlsSizer->Add(delAtom, { 9,0 }, {1,2},wxALIGN_CENTER | wxALL, FromDIP(5));
	//controlsSizer->AddStretchSpacer(1);

	auto mainSizer = new wxBoxSizer(wxHORIZONTAL);


	mainSizer->Add(oglPane,5, wxALL | wxEXPAND, FromDIP(10));

	mainSizer->Add(rightPanel, 0, wxALL | wxEXPAND, FromDIP(10));
	
	rightPanel->SetSizerAndFit(controlsSizer);
	this->SetSizerAndFit(mainSizer);
	this->SetSize(wxSize(800, 600));
	this->SetBackgroundColour(wxColour(230, 230, 230));

	timer = new RenderTimer(oglPane);
	timer->start();

	timer->mtx = mtx;
	oglPane->silicon->mtx = mtx;

	//ofstream myfile("energy.txt");
	//myfile.close();

	//ofstream myfile2("temp.txt");
	//myfile2.close();
}

void MyFrame::OnApplySize(wxCommandEvent& event) {
	int size{};
	if (!cellSize->GetValue().ToInt(&size)) {
		return;
	}
	oglPane->silicon->Init(size);
}

void MyFrame::OnRandPos(wxCommandEvent &event) {
	oglPane->silicon->RandomAtomsShift();
}

void MyFrame::OnRandVel(wxCommandEvent& event) {
	oglPane->silicon->RandomAtomVelocity();
}

void MyFrame::OnReset(wxCommandEvent& event) {
	oglPane->silicon->Reset();

	//ofstream myfile("energy.txt");
	//myfile.close();

	//ofstream myfile2("temp.txt");
	//myfile2.close();
}

void MyFrame::OnStep(wxCommandEvent& event) {

	if (!t1) {
		bCalcInProc = true;
		t1 = new thread(&MyFrame::CalcVerlet, std::ref(*this));
		step->SetLabelText("����");
	}
	else {
		bCalcInProc = false;
		t1->join();
		delete t1;
		t1 = nullptr;
		step->SetLabelText("�����");
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