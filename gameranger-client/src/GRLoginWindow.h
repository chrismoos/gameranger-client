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

class GRProfile;

class GRLoginWindow : public wxFrame
{
	public:
		GRLoginWindow(const wxString &title, const wxPoint &pos, const wxSize &size);
		~GRLoginWindow();

		//GUI Functions
		void createControls();

		//GUI Controls
		wxTextCtrl *emailEdit;
		wxTextCtrl *passwordEdit;
		wxButton *loginButton;
		wxComboBox *comboBox;

		//Events
		void OnLoginButton(wxCommandEvent &event);
		void OnComboBoxSelect(wxCommandEvent &event);
		
		GRMainWindow *mainWindow;

		//Profiles
		vector<GRProfile*> Profiles;
		void loadProfiles();
		GRProfile *currentProfile;

		DECLARE_EVENT_TABLE()
};

enum LOGIN_CONTROLS {
	LOGIN_BUTTON = wxID_HIGHEST+20,
	PROFILE_COMBO_BOX
};

#endif
