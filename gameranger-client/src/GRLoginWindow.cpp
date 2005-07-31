/*
GameRanger
GRLoginWindow.cpp

Copyright (C) 2004 xoclipse (xoclipse@cox.net)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "GRLoginWindow.h"
#include "GRLogger.h"
#include "GRProfile.h"
#include "GRApplication.h"
#include "memdebug.h"

/* Login Window instance */
GRLoginWindow *GRLoginWindow::_instance = NULL;

GRLoginWindow::GRLoginWindow(const wxFrame *parent, const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) parent, -1, title, pos, size)
{
	/* Set some generic window options */
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	/* Create our controls */
	createControls();
	
	/* Load Profiles */
	profileManager = GRProfileManager::getInstance();
	populateProfilesList();

	/* Center window */
	CentreOnScreen();
}
/*------------------------------------------------------------------------------------*/
GRLoginWindow::~GRLoginWindow()
{
	_instance = NULL;
}
/*------------------------------------------------------------------------------------*/
GRLoginWindow *GRLoginWindow::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRLoginWindow(NULL, wxT("Login to GameRanger"), wxDefaultPosition,
			wxSize(200, 200));
	}
	return _instance;	
}
/*------------------------------------------------------------------------------------*/
GRLoginWindow *GRLoginWindow::getInstance(wxFrame *parent)
{
	if(_instance == NULL) {
		_instance = new GRLoginWindow(parent, wxT("Login to GameRanger"), wxDefaultPosition,
			wxSize(200, 200));
	}
	return _instance;	
}
/*------------------------------------------------------------------------------------*/
void GRLoginWindow::createControls()
{
	//Main sizer for the window
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	//Panel
	wxPanel *panel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize);

	//flex grid sizer
	wxFlexGridSizer *gridSizer = new wxFlexGridSizer(3, 2, 10, 10); 

	//Button's sizer
	wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

	/* --------------- Create Email and Password fields and static texts ---------- */
	//combo box
	comboBox = new wxComboBox(panel, PROFILE_COMBO_BOX, wxT(""), wxDefaultPosition, wxSize(150, 20),
		0, NULL, wxCB_READONLY);

	emailEdit = new wxTextCtrl(panel, -1, wxT(""), wxDefaultPosition, wxSize(150, 20));
	emailEdit->SetMaxLength(32);
	passwordEdit = new wxTextCtrl(panel, -1, wxT(""), wxDefaultPosition, wxSize(150, 20), wxTE_PASSWORD);
	passwordEdit->SetMaxLength(32);
	wxStaticText *profileText = new wxStaticText(panel, -1, wxT("Profile: "));
	wxStaticText *emailStatic = new wxStaticText(panel, -1, wxT("E-Mail Address: "));
	wxStaticText *passStatic = new wxStaticText(panel, -1, wxT("Password: "));

	gridSizer->Add(profileText, 0, wxALIGN_LEFT);
	gridSizer->Add(comboBox, 0, wxALIGN_CENTER);
	gridSizer->Add(emailStatic, 0, wxALIGN_LEFT);
	gridSizer->Add(emailEdit, 0, wxALIGN_CENTER);
	gridSizer->Add(passStatic, 0, wxALIGN_LEFT);
	gridSizer->Add(passwordEdit, 0, wxALIGN_CENTER);

	savePass = new wxCheckBox(panel, -1, wxT("Save password"), wxDefaultPosition, wxDefaultSize);


	/* ------------------- Create Buttons ------------------------------------------ */
	wxButton *loginButton = new wxButton(panel, LOGIN_BUTTON, wxT("Login"), wxDefaultPosition, wxDefaultSize);
	bottomSizer->Add(loginButton, 0, wxALIGN_CENTER | wxRIGHT, 5);


	/* ---------------- Add to Main Sizer ------------------------------------------ */
	mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 15);
	mainSizer->Add(savePass, 0, wxALIGN_CENTER | wxBOTTOM, 7);
	mainSizer->Add(bottomSizer, 0, wxALIGN_CENTER | wxBOTTOM | wxRIGHT | wxLEFT, 15);


	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(panel);
	mainSizer->SetSizeHints(this);
}
/*-----------------------------------------------------------------------------------*/
void GRLoginWindow::populateProfilesList()
{
	vector<GRProfile*> profileList = profileManager->getProfiles();
	unsigned int x;

	selectedProfile = NULL;

	/* Delete current profiles from list */
	comboBox->Clear();

	/* Add New profile option */
	comboBox->Append(wxT("New Profile"), (void*)NULL);
	comboBox->SetSelection(0);

	/* Add all regular profiles */
	for(x = 0; x < profileList.size(); x++) {
		comboBox->Append(profileList[x]->nickname, (void*)profileList[x]);
		if(x == 0) {
			comboBox->SetSelection(1);
			selectedProfile = profileList[x];
			/* Put profile values into according edit boxes */
			emailEdit->SetValue(profileList[x]->email);
			if(profileList[x]->savePass) {
				passwordEdit->SetValue(profileList[x]->password);
				savePass->SetValue(true);
			}
			else {
				passwordEdit->SetValue(wxT(""));
			}
		}
	}
}
/*-----------------------------------------------------------------------------------*/
void GRLoginWindow::OnLoginButton(wxCommandEvent &event)
{
	wxMessageDialog *dlg;
	if(comboBox->GetSelection() == -1) 
	{
		dlg = new wxMessageDialog(this, wxT("Please chose a valid profile. If you are creating a new profile, please select \"New Profile\" from the drop down box above"), wxT("Error"), wxICON_ERROR);
		dlg->ShowModal();
		delete(dlg);
		return;
	}
	if(emailEdit->GetValue().Len() == 0) 
	{
		dlg = new wxMessageDialog(this, wxT("Please enter a valid e-mail address."), wxT("Login error"), wxICON_ERROR);
		dlg->ShowModal();
		delete(dlg);
		return;
	}
	else if(passwordEdit->GetValue().Len() == 0) 
	{
		dlg = new wxMessageDialog(this, wxT("Please enter a valid password."), wxT("Login error"), wxICON_ERROR);
		dlg->ShowModal();
		delete(dlg);
		return;
	}
	if(selectedProfile == NULL) {
		GRApplication::getInstance()->Login(emailEdit->GetValue(), passwordEdit->GetValue(), savePass->IsChecked());
	}
	else {
		selectedProfile->savePass = savePass->IsChecked();
		selectedProfile->password = passwordEdit->GetValue();
		GRApplication::getInstance()->Login(selectedProfile);
	}
}
/*------------------------------------------------------------------------------------*/
void GRLoginWindow::OnComboBoxSelect(wxCommandEvent &event)
{
	GRProfile *profile = NULL;

	profile = (GRProfile*)comboBox->GetClientData(event.GetSelection());

	if(profile == NULL) { /* Default profile, zero stuff out */
		emailEdit->SetValue(wxT(""));
		passwordEdit->SetValue(wxT(""));
		savePass->SetValue(false);
		selectedProfile = NULL;
		return;
	}

	/* Put profile values into according edit boxes */
	emailEdit->SetValue(profile->email);
	if(profile->savePass) {
		passwordEdit->SetValue(profile->password);
		savePass->SetValue(true);
	}
	else {
		passwordEdit->SetValue(wxT(""));
	}

	/* we've selected this profile */
	selectedProfile = profile;
}
/*------------------------------------------------------------------------------------*/

