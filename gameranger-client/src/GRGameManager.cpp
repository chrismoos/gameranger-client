/*
GameRanger
GRGameManager.cpp

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

#include "GRGameManager.h"
#include "GRLogger.h"
#include "GRMainWindow.h"
#include "GRApplication.h"

GRGameManager::GRGameManager(GRMainConnection *conn)
{
	this->conn = conn;
}
/*-------------------------------------------------------------------*/
GRGameManager::~GRGameManager()
{
	for(unsigned int x = 0; x < gameRooms.size(); x++) delete(gameRooms[x]);
	gameRooms.clear();
}
/*-------------------------------------------------------------------*/
void GRGameManager::clearAll()
{
	/* remove all game rooms from list, and delete them */
	for(unsigned int x = 0; x < gameRooms.size(); x++) delete(gameRooms[x]);
	gameRooms.clear();

	/* remove items from the list view in the main window */
	GRApplication::getInstance()->getMainWindow()->clearGameRooms();
}
/*-------------------------------------------------------------------*/
void GRGameManager::addRoom(GRGameRoom *gameRoom)
{
	if(gameRoom == NULL) return;

	/* add to list of games */
	gameRooms.push_back(gameRoom);

	/* add to list view in main window */
	GRApplication::getInstance()->getMainWindow()->addGameRoomToListView(gameRoom);
}
/*-------------------------------------------------------------------*/
void GRGameManager::addRoomAndNotify(GRGameRoom *gameRoom)
{
	if(gameRoom == NULL) return;

	/* add game room */
	addRoom(gameRoom);

	/* notify */
	GRApplication::getInstance()->getMainWindow()->displayGameRoomOpened(gameRoom);
}
/*-------------------------------------------------------------------*/
void GRGameManager::removeRoom(wxUint32 roomID)
{
	GRGameRoom *room = findRoom(roomID);
	if(room == NULL) return;

	removeRoom(room);
}
/*-------------------------------------------------------------------*/
GRGameRoom *GRGameManager::findRoom(wxUint32 roomID)
{
	for(unsigned int x = 0; x < gameRooms.size(); x++) {
		if(gameRooms[x]->gameRoomID == roomID) return gameRooms[x];
	}
	return NULL;
}
/*-------------------------------------------------------------------*/
void GRGameManager::removeRoom(GRGameRoom *room)
{
	vector <GRGameRoom*>::iterator it;
	if(room == NULL) return;

	/* remove from main window list view */
	GRApplication::getInstance()->getMainWindow()->removeGameFromListView(room);

	/* notify in main window */
	GRApplication::getInstance()->getMainWindow()->displayGameRoomClosed(room);

	/* remove room from list */
	for(unsigned int x = 0; x < gameRooms.size(); x++) {
		if(gameRooms[x] == room) {
			it = gameRooms.begin()+x;
			gameRooms.erase(it);
			break;
		}
	}
}
/*-------------------------------------------------------------------*/
void GRGameManager::incrementRoom(GRGameRoom *room)
{
	if(room == NULL) return;

	/* Increment current players */
	room->currentPlayers++;

	/* Update in main window list view */
	GRApplication::getInstance()->getMainWindow()->updateGameRoomInListView(room);
}
/*-------------------------------------------------------------------*/
void GRGameManager::incrementRoom(wxUint32 roomID)
{
	GRGameRoom *room = findRoom(roomID);
	if(room == NULL) return;

	incrementRoom(room);
}
/*-------------------------------------------------------------------*/
void GRGameManager::decrementRoom(GRGameRoom *room)
{
	if(room == NULL) return;

	/* Increment current players */
	room->currentPlayers--;

	/* Update in main window list view */
	GRApplication::getInstance()->getMainWindow()->updateGameRoomInListView(room);
}
/*-------------------------------------------------------------------*/
void GRGameManager::decrementRoom(wxUint32 roomID)
{
	GRGameRoom *room = findRoom(roomID);
	if(room == NULL) return;

	incrementRoom(room);
}
/*-------------------------------------------------------------------*/
void GRGameManager::gameStatusChanged(GRGameRoom *room, int status)
{
	if(room == NULL) return;

	if(status == 0) { /* not playing.. */
		room->status -= 8;
	}
	else {
		room->status += 8;
	}

	/* update list item */
	GRApplication::getInstance()->getMainWindow()->setGameRoomListInfo(room);
}
/*-------------------------------------------------------------------*/
void GRGameManager::gameStatusChanged(wxUint32 roomID, int status)
{
	GRGameRoom *gameRoom = findRoom(roomID);
	if(gameRoom != NULL) gameStatusChanged(gameRoom, status);
}
/*-------------------------------------------------------------------*/

