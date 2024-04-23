#pragma once


#include "wx\wx.h"
#include "wx\button.h"
#include "MyFrame.h"
#include "OGLPane.h"
#include "Silicon.h"
#include "RenderTimer.h"

#include <thread>
#include <mutex>

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
	OGLPane* oglPane{};
	RenderTimer* timer{};
	wxTextCtrl* cellSize{};
	wxTextCtrl* atomShift{};
	wxTextCtrl* atomVel{};
	wxTextCtrl* rlxCoef{};
	wxButton* applySize{};
	wxButton* step{};
	wxButton* rndPos{};
	wxButton* rndVel{};
	wxButton* reset{};
	wxButton* delAtom{};
	wxCheckBox* rlxCheck{};
	mutex* mtx{};
	thread* t1{};
	bool bRlx{ false };
	void OnApplySize(wxCommandEvent& event);
	void OnStep(wxCommandEvent& event);
	void OnRandPos(wxCommandEvent& event);
	void OnRandVel(wxCommandEvent& event);
	void OnReset(wxCommandEvent& event);
	void OnDeleteAtom(wxCommandEvent& event);
	void OnChecked(wxCommandEvent& event);
public:
	void CalcVerlet();
	bool bCalcInProc;
};

