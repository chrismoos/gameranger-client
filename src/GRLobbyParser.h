/*
GameRanger
GRLobbyParser.h

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

#ifndef GRLOBBYPARSER_H
#define GRLOBBYPARSER_H

#include "GRBaseDefs.h"
#include "GRUser.h"
#include "GRMainConnection.h"

class GRLobbyParser {
public:
	GRLobbyParser(GRMainConnection *conn);
	~GRLobbyParser();

	/* Run parser */
	bool run(GR_PACKET *Packet);

	/* List of lobbies */
	void publicLobbyList(GR_PACKET *Packet);

	/* User list for current lobby */
	void lobbyUserList(GR_PACKET *Packet);
	void requestMissingIconsFromUserList(wxUint32 count);

	/* User action in lobby */
	void lobbyUserAction(GR_PACKET *Packet);

	/* Parse user joined lobby extra data */
	void parseUserJoinedLobby(wxUint32 previousLobby, wxUint32 newLobby, wxUint32 userID, wxUint8 *buf);
	
	/* User was disconnected */
	void userLeftServer(GR_PACKET *Packet);

	/* Chat Message */
	void parseChatMessage(GR_PACKET *Packet, int type);

	/* User changed nickname */
	void parseUserChangedNick(GR_PACKET *Packet);

	/* Status change */
	void parseUserIsIdle(GR_PACKET *Packet);
	void parseUserIsActive(GR_PACKET *Packet);

private:
	GRMainConnection *conn;
};

#endif
