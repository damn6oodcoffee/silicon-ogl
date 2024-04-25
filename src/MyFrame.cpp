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
	oglPane->Init(defaultCellSize);
	oglPane->SetMutex(mtx);
	oglPane->SetWindowStyleFlag(wxSUNKEN_BORDER);
	
	wxIntegerValidator<int> sizeValidator{};
	sizeValidator.SetRange(1, 5);
	wxFloatingPointValidator<double> scaleValidator(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
	scaleValidator.SetRange(0.0, 100.0);
	wxFloatingPointValidator<double> coefValidator(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
	coefValidator.SetRange(0.0, 1.0);

	
	cellSize = new wxTextCtrl(rightPanel, wxID_ANY, wxString::Format(wxT("%i"), defaultCellSize));
	cellSize->SetValidator(sizeValidator);
	atomShift = new wxTextCtrl(rightPanel, wxID_ANY, "0.02");
	atomShift->SetValidator(scaleValidator);
	atomVel = new wxTextCtrl(rightPanel, wxID_ANY, "1");
	atomVel->SetValidator(scaleValidator);
	rlxCoef = new wxTextCtrl(rightPanel, wxID_ANY, "0.8");
	rlxCoef->SetValidator(coefValidator);

	applySize = new wxButton(rightPanel, wxID_ANY, "Применить");
	rndPos = new wxButton(rightPanel, wxID_ANY, "Задать случайны\nсдвиг");
	rndVel = new wxButton(rightPanel, wxID_ANY, "Задать случайную\nскорость");
	reset = new wxButton(rightPanel, wxID_ANY, "Сброс");
	delAtom = new wxButton(rightPanel, wxID_ANY, "Удалить центральный\nатом");
	step = new wxButton(rightPanel, wxID_ANY, "Старт");
	rlxCheck = new wxCheckBox(rightPanel, wxID_ANY, "Релаксация");
	
	applySize->Bind(wxEVT_BUTTON, &MyFrame::OnApplySize, this);
	rndPos->Bind(wxEVT_BUTTON, &MyFrame:: OnRandPos, this);
	rndVel->Bind(wxEVT_BUTTON, &MyFrame::OnRandVel, this);
	reset->Bind(wxEVT_BUTTON, &MyFrame::OnReset, this);
	delAtom->Bind(wxEVT_BUTTON, &MyFrame::OnDeleteAtom, this);
	step->Bind(wxEVT_BUTTON, &MyFrame::OnStep, this);
	rlxCheck->Bind(wxEVT_CHECKBOX, &MyFrame::OnChecked, this);
	
	controlsSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "Размер расчетной ячейки:"), { 0,0 }, { 1,1 }, wxALIGN_CENTER);
	controlsSizer->Add(cellSize, { 1,0 }, { 1,1 }, wxALIGN_CENTER);
	controlsSizer->Add(applySize, { 1,1 }, { 1,1 }, wxALIGN_CENTER | wxLEFT | wxRIGHT, FromDIP(5));
	controlsSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "Величина случайного сдвига\n(в ед. парам-ра решетки):", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), { 2,0 }, { 1,1 }, wxALIGN_CENTER);
	controlsSizer->Add(atomShift, { 3,0 }, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(rndPos, { 3,1 }, {1,1}, wxALIGN_CENTER | wxLEFT | wxRIGHT, FromDIP(5));
	controlsSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "Величина случайной скорости\n(в ед. характерной ск-ти):", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), { 4,0 }, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(atomVel, {5,0}, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(rndVel, { 5,1 }, {1,1}, wxALIGN_CENTER | wxLEFT | wxRIGHT, FromDIP(5));
	controlsSizer->Add(new wxStaticText(rightPanel, wxID_ANY, "Коэф. релаксации:"), { 6,0 }, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(rlxCoef, { 7,0 }, {1,1},  wxALIGN_CENTER);
	controlsSizer->Add(rlxCheck, { 7,1 }, {1,1}, wxALIGN_CENTER);
	controlsSizer->Add(step, { 8,0 }, {1,1}, wxALIGN_CENTER | wxALL, FromDIP(5));
	controlsSizer->Add(reset, { 8, 1 }, {1,1}, wxALIGN_CENTER | wxALL, FromDIP(5));
	controlsSizer->Add(delAtom, { 9,0 }, {1,2},wxALIGN_CENTER | wxALL, FromDIP(5));


	auto mainSizer = new wxBoxSizer(wxHORIZONTAL);


	mainSizer->Add(oglPane,5, wxALL | wxEXPAND, FromDIP(10));

	mainSizer->Add(rightPanel, 0, wxALL | wxEXPAND, FromDIP(10));
	
	rightPanel->SetSizerAndFit(controlsSizer);
	this->SetSizerAndFit(mainSizer);
	this->SetSize(wxSize(1280, 800));
	this->SetBackgroundColour(wxColour(230, 230, 230));

	timer = new RenderTimer(oglPane);
	timer->start();
	timer->mtx = mtx;

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

void MyFrame::OnRandPos(wxCommandEvent& event) {
	double posShift{};
	if (!atomShift->GetValue().ToDouble(&posShift)) {
		return;
	}
	oglPane->silicon->RandomAtomsShift(posShift);
}

void MyFrame::OnRandVel(wxCommandEvent& event) {
	double velShift{};
	if (!atomVel->GetValue().ToDouble(&velShift)) {
		return;
	}
	oglPane->silicon->RandomAtomVelocity(velShift);
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
		applySize->Disable();
		reset->Disable();
		delAtom->Disable();
		step->SetLabelText("Стоп");
	}
	else {
		bCalcInProc = false;
		t1->join();
		delete t1;
		t1 = nullptr;
		applySize->Enable();
		reset->Enable();
		delAtom->Enable();
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
	double coef{};
	if (!rlxCoef->GetValue().ToDouble(&coef)) {
		return;
	}
	bRlx = !bRlx;
	oglPane->silicon->alpha_rlx = coef;
	oglPane->silicon->bRlx = bRlx;
}