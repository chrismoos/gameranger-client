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
#include "GRLogWindow.h"
#include "GRProfile.h"
#include "memdebug.h"

GRLoginWindow::GRLoginWindow(const wxFrame *parent, const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) parent, -1, title, pos, size)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	mainWindow = NULL;


	//Create our controls
	createControls();

	
	//Load Profiles
	loadProfiles();
	currentProfile = NULL;

	//Center window
	CentreOnScreen();
}
//-------------------------------------------------------------------------------
GRLoginWindow::~GRLoginWindow()
{
	wxUint32 x;
	for(x = 0; x < Profiles.size(); x++) delete(Profiles[x]);
	if(mainWindow != NULL) 
	{
		mainWindow->loginWindow = NULL;
	}
}
//-------------------------------------------------------------------------------
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

	/* ------------------- Create Buttons ------------------------------------------ */
	wxButton *loginButton = new wxButton(panel, LOGIN_BUTTON, wxT("Login"), wxDefaultPosition, wxDefaultSize);
	bottomSizer->Add(loginButton, 0, wxALIGN_CENTER | wxRIGHT, 5);


	/* ---------------- Add to Main Sizer ------------------------------------------ */
	mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 15);
	mainSizer->Add(bottomSizer, 0, wxALIGN_CENTER | wxBOTTOM | wxRIGHT | wxLEFT, 15);


	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(panel);
	mainSizer->SetSizeHints(this);
}
//--------------------------------------------------------------------------------
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
	if(currentProfile == NULL) {
		currentProfile = new GRProfile();
		Profiles.push_back(currentProfile);
	}
	mainWindow->myUserID = currentProfile->grID;
	mainWindow->myNickname = currentProfile->nickname;
	mainWindow->currentProfile = currentProfile;
	mainWindow->Login((char*)(const char*)emailEdit->GetValue().mb_str(), (char*)(const char*)passwordEdit->GetValue().mb_str());

}
//---------------------------------------------------------------------------------
void GRLoginWindow::loadProfiles()
{
	wxDir dir;
	wxString filename;
	GRProfile *profile;
	wxInt32 index;

	if(!dir.Exists(wxGetCwd()+wxT("/profiles"))) {
		wxMessageDialog *dlg = new wxMessageDialog(this, wxT("Error: Profile directory does not exist. Please create it and try again."), wxT("Directory Error"), wxICON_ERROR);
		dlg->ShowModal();
		delete(dlg);
		return;
	}
		
	dir.Open(wxGetCwd()+wxT("/profiles"));

	if(!dir.IsOpened()) 
	{
		return;
	}

	bool cont = dir.GetFirst(&filename, wxT(""), wxDIR_FILES);
	while(cont)
	{
		profile = new GRProfile();
		profile->Read(wxGetCwd()+wxT("/profiles/")+filename);
		profile->comboIndex = comboBox->GetCount();
		comboBox->Append(profile->nickname);
		Profiles.push_back(profile);
		cont = dir.GetNext(&filename);
	}
	profile = new GRProfile();
	profile->comboIndex = comboBox->GetCount();
	profile->email = wxT("");
	index = comboBox->GetCount();
	comboBox->Append(wxT("New Profile"));
	Profiles.push_back(profile);
}
//------------------------------------------------------------------------------
void GRLoginWindow::OnComboBoxSelect(wxCommandEvent &event)
{
	int x, index;
	GRProfile *profile = NULL;

	index = comboBox->GetSelection();
	if(index == -1) return;

	for(x = 0; x < Profiles.size(); x++)
	{
		if(Profiles[x]->comboIndex == index)
		{
			profile = Profiles[x];
			break;
		}
	}

	if(profile == NULL) return;

	currentProfile = profile;

	emailEdit->SetValue(profile->email);

}
//------------------------------------------------------------------------------
