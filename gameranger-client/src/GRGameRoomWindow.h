/*
GameRanger
GRGameRoomWindow.h

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

#ifndef GRGAMEROOMWINDOW_H
#define GRGAMEROOMWINDOW_H

#include "GRBaseDefs.h"

class GRMainWindow;
class GRUser;
class GRGameRoom;

using namespace std;

class GRGameRoomWindow : public wxFrame
{
public:
	GRGameRoomWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size);
	~GRGameRoomWindow();

	//GUI Controls
	wxTextCtrl *chatWindow;
	wxTextCtrl *chatEditField;
	wxListCtrl *userListBox;

	//Events
	void OnChatEditEnter(wxCommandEvent &event);
	void OnUserDoubleClick(wxListEvent& event);

	//Main Window
	GRMainWindow *mainWindow;

	//Game room pointer
	GRGameRoom *gameRoom;

	//Users
	vector <GRUser*> users;
	void AddUser(GRUser *user, bool notify);
	void userJoined(GR_PACKET *Packet);
	GRUser *findUser(wxUint32 userID);
	void userLeft(GR_PACKET *Packet);
	void RemoveUser(GRUser *User);

	//Chat
	void chatMessage(GR_PACKET *Packet, int type);

	//Helpers
	void addTextWithColor(wxString str, wxColour color);
	void setListInfo(GRUser *User);
	wxInt32 getUserItemIndex(GRUser *user);

	//create controls
	void createControls();

	DECLARE_EVENT_TABLE()
};

enum {
	GAMEROOM_CHATEDIT_ID = wxID_HIGHEST+120,
	GAMEROOM_CHATLOG_ID,
	GAMEROOM_USERLIST_ID
};

#endif
