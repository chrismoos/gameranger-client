/*
GameRanger
GRLobby.h

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

#ifndef GRLOBBY_H
#define GRLOBBY_H

#include "GRBaseDefs.h"

class GRUser;
class GRMainWindow;

using namespace std;

class GRLobby 
{
public:
	GRLobby(wxString name, wxUint32 ID, wxUint8 type);
	~GRLobby();

	//Window
	GRMainWindow *mainWindow;

	//Lobby Info
	wxString lobbyName;
	wxUint8 lobbyType;
	wxUint32 lobbyID;
	wxUint32 userCount;

	//Lobby GUI Stuff
	wxInt32 comboIndex;

	//Users
	vector<GRUser*> Users;
	GRUser *findUser(wxUint32 userID);

	//Clearing
	void ClearUsers();

	//Join/leave
	void AddUser(GRUser *User, int type);
	void userAction(GR_PACKET *Packet);
	void RemoveUser(GRUser *User, int type, wxString reason);
	void userLeftServer(GR_PACKET *Packet);

	//Changing nickname
	void userChangedNick(GR_PACKET *Packet);

	//Idle/Active
	void userIsIdle(GR_PACKET *Packet);
	void userIsActive(GR_PACKET *Packet);

	//Chat
	void chatMessage(GR_PACKET *Packet, int type);

	//Lobby Count
	void updateComboString();

	void SetUserCount(wxUint32 count);
};	



#endif
