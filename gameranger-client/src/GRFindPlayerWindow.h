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

#ifndef GRFINDPLAYER_WINDOW_H
#define GRFINDPLAYER_WINDOW_H

#include "GRBaseDefs.h"
#include "GRMainWindow.h"

class GRFindPlayerWindow : public wxFrame
{
public:
	GRFindPlayerWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size);
	~GRFindPlayerWindow();

	//gui elements
	wxTextCtrl *m_accountValue;
	wxTextCtrl *m_nickValue;
	wxTextCtrl *m_emailValue;
	wxTextCtrl *m_userIDValue;
	wxButton *m_findButton;
	wxListView *m_resultsList;
	wxStaticBoxSizer *resultsSizer;

	//search functions
	void searchByAccount();
	void searchByNick();
	void searchByEmail();
	void searchByID();

	//results
	void findUserResults(GR_PACKET *Packet);
	

	//create controls
	void createControls();

	//main window
	GRMainWindow *m_mainWindow;

	//Events
	void OnSearch(wxCommandEvent &event);
	void OnUserDoubleClick(wxListEvent& event);

	DECLARE_EVENT_TABLE()
};

enum {
	SEARCH_BUTTON_ID = 1050,
	RESULTS_LIST_ID
};

#endif
