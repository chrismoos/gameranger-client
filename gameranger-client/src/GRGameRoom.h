/*
GameRanger
GRGameRoom.h

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

#ifndef GRGAMEROOM_H
#define GRGAMEROOM_H

#include "GRBaseDefs.h"

class GRMainWindow;
class GRPlugin;

class GRGameRoom
{
public:
	GRGameRoom(wxUint32 gameRoomID, GRPlugin *Plugin, wxUint32 maxPlayers, wxUint32 currentPlayers, wxString ip, wxUint8 locked, wxString description, wxUint32 grID, wxUint32 iconID, wxUint32 status, wxString host);
	~GRGameRoom();

	GRPlugin *Plugin;

	//Room Info
	wxUint32 gameRoomID;
	wxUint32 maxPlayers;
	wxUint32 currentPlayers;
	wxString description;
	wxUint8 locked;

	//Host info
	wxString ip, host;
	wxUint8 status;
	wxUint32 grID, iconID;

	//Are they?
	bool isPremium();
	bool isPlaying();
	bool isIdle();

};

#endif
