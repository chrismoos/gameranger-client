/*
GameRanger
GRPrivateMessage.h

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

#ifndef GRPRIVATEMESSAGE_H
#define GRPRIVATEMESSAGE_H

#include "GRBaseDefs.h"

class GRUser;
class GRMainWindow;

class GRPrivateMessage : public wxFrame
{
public:
	GRPrivateMessage(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size);
	~GRPrivateMessage();

	//GUI Controls
	wxTextCtrl *chatWindow;
	wxTextCtrl *chatEditField;

	//Main Window
	GRMainWindow *mainWindow;

	//Create Controls
	void createControls();

	//Events
	void OnChatEditEnter(wxCommandEvent &event);

	//Helpers
	void addTextWithColor(wxString str, wxColour color);

	//User info
	wxString nickname;
	wxUint32 userID;

	DECLARE_EVENT_TABLE()
};

enum {
	PM_CHATLOG_ID = 100,
	PM_CHATEDIT_ID
};

#endif
