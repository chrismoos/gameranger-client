/*
GameRanger
GRUser.h

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

#ifndef GRUSER_H
#define GRUSER_H

#include "GRBaseDefs.h"

class GRMainWindow;
class GRLobby;
class GRIcon;
class GRPlugin;

using namespace std;

class GRUser
{
public:
	GRUser(wxString nick, wxUint32 userID, wxUint32 iconID);
	GRUser();
	~GRUser();

	wxString nick;
	wxUint32 userID;
	wxUint32 iconID;
	wxUint8 status;
	GRIcon *icon;

	GRLobby *currentLobby;

	void SetStatus(wxUint8 status);

	//Games list
	void addGameToList(GRPlugin *game);
	void parseGamesList(wxUint8 *buf);
	vector <GRPlugin*> gamesList;

	//Status
	bool isPremium();
	bool isPlaying();
	bool isIdle();
	void setIdle();
	void setActive();
};

#endif
