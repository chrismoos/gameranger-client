/*
GameRanger
GRHostWindow.h

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

#ifndef GRHOSTWINDOW_H
#define GRHOSTWINDOW_H

#include "GRBaseDefs.h"

class GRMainWindow;
class GRUser;
class GRGameRoom;

using namespace std;

class GRHostWindow : public wxFrame
{
public:
	GRHostWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size);
	~GRHostWindow();

	/* GUI Controls */
	wxComboBox *gameCombo;
	wxComboBox *playersCombo;
	wxTextCtrl *descText;
	wxTextCtrl *passTextBox;
	wxCheckBox *lateJoinersCheck;

	/* Events */
	void OnHostButton(wxCommandEvent &evt);
	void OnGameComboBoxSelect(wxCommandEvent &evt);


	/* Main Window */
	GRMainWindow *mainWindow;

	/* Game room pointer */
	GRGameRoom *gameRoom;

	/* init list boxes */
	void initListBoxes();


	//create controls
	void createControls();

	DECLARE_EVENT_TABLE()
};

enum {
	HOST_BUTTON_ID = wxID_HIGHEST+180,
	HOST_GAME_COMBO_LIST,
	PLAYERS_GAME_COMBO_LIST
};

#endif
