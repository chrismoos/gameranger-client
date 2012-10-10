/*
GameRanger
GRFindPlayerWindow.h

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

#ifndef GRCHANGEMYGAMES_WINDOW_H
#define GRCHANGEMYGAMES_WINDOW_H

#include "GRBaseDefs.h"
#include "GRMainWindow.h"

using namespace std;

class GRChangeMyGames : public wxFrame
{
public:
	GRChangeMyGames(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size);
	~GRChangeMyGames();

	//gui elements
	wxCheckListBox *m_listBox;
	wxButton *m_saveButton;

	//populate with games
	void populateListBox();	

	//create controls
	void createControls();

	//Events
	void OnSaveGameList(wxCommandEvent &event);
	void OnCheckAll(wxCommandEvent &event);
	void OnUncheckAll(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};

enum {
	SAVE_GAME_LIST_BUTTON_ID = 1070,
	CHECK_ALL_BUTTON_ID, 
	UNCHECK_ALL_BUTTON_ID
};

#endif
