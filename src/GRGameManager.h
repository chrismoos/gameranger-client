/*
GameRanger
GRGameManager.h

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

#ifndef GRGAMEMANAGER_H
#define GRGAMEMANAGER_H

#include "GRBaseDefs.h"
#include "GRMainConnection.h"
#include "GRGameRoom.h"

using namespace std;

class GRGameManager {
public:
	GRGameManager(GRMainConnection *conn);
	~GRGameManager();

	/* Add game room */
	void addRoom(GRGameRoom *gameRoom);
	void addRoomAndNotify(GRGameRoom *gameRoom);

	/* Remove a game room */
	void removeRoom(wxUint32 roomID);
	void removeRoom(GRGameRoom *room);

	/* Find a game room */
	GRGameRoom *findRoom(wxUint32 roomID);

	/* Room Increment/decrement */
	void incrementRoom(GRGameRoom *room);
	void incrementRoom(wxUint32 roomID);
	void decrementRoom(wxUint32 roomID);
	void decrementRoom(GRGameRoom *room);

	/* Game Room Status */
	void gameStatusChanged(wxUint32 roomID, int status);
	void gameStatusChanged(GRGameRoom *room, int status);

	/* Clear all rooms */
	void clearAll();

private:
	GRMainConnection *conn;

	vector <GRGameRoom*> gameRooms;
};

#endif
