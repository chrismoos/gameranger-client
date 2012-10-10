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
	void clearAll();
	void ClearUsers();

	//Join/leave
	void addUser(GRUser *User, int type);
	void removeUserByID(wxUint32 userID, int type, wxString reason);
	void removeUser(GRUser *user, int type, wxString reason);
	void joinLobby(); /* try to join it */

	void AddUser(GRUser *User, int type);
	void userAction(GR_PACKET *Packet);
	void RemoveUser(GRUser *User, int type, wxString reason);
	
	/* Manage user count */
	void decrementUserCount();
	void incrementUserCount();

	/* icons */
	void purgeIcons();
	void userChangedIcon(wxUint32 userID, wxUint32 iconID);

	//Changing nickname
	void userChangedNick(wxUint32 userID, wxString newNick);
	void userChangedNick(GRUser *user, wxString newNick);

	//Idle/Active
	void userIsIdle(wxUint32 userID);
	void userIsIdle(GRUser *user);
	void userIsActive(wxUint32 userID);
	void userIsActive(GRUser *user);

	//Chat
	void chatMessage(wxUint32 userID, wxString message, int type);
	void chatMessage(GRUser *user, wxString Message, int type);

	//Lobby Count
	void updateComboString();

	void SetUserCount(wxUint32 count);
};	



#endif
