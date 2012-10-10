/*
GameRanger
GRLoginWindow.h

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

#ifndef GRLOGINWINDOW_H
#define GRLOGINWINDOW_H

#include "GRBaseDefs.h"
#include "GRConnectStatusWindow.h"
#include "GRMainWindow.h"
#include "GRProfileManager.h"

class GRProfile;

class GRLoginWindow : public wxFrame
{
private:
	static GRLoginWindow *_instance;

	GRProfileManager *profileManager;

protected:
	GRLoginWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size);

public:
	~GRLoginWindow();

	/* Get Instance of login window */
	static GRLoginWindow *getInstance();
	static GRLoginWindow *getInstance(wxFrame *parent);

	//GUI Functions
	void createControls();

	//GUI Controls
	wxTextCtrl *emailEdit;
	wxTextCtrl *passwordEdit;
	wxButton *loginButton;
	wxComboBox *comboBox;
	wxCheckBox *savePass;
	
	//Events
	void OnLoginButton(wxCommandEvent &event);
	void OnComboBoxSelect(wxCommandEvent &event);

	/* Profiles */
	void populateProfilesList();
	GRProfile *selectedProfile;
	GRProfile *currentProfile;

	DECLARE_EVENT_TABLE()
};

enum LOGIN_CONTROLS {
	LOGIN_BUTTON = wxID_HIGHEST+20,
	PROFILE_COMBO_BOX
};

#endif
