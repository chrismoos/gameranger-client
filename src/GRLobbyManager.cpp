/*
GameRanger
GRLobbyManager.cpp

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

#include "GRLobbyManager.h"
#include "GRConnection.h"
#include "GRApplication.h"
#include "GRLogger.h"

GRLobbyManager::GRLobbyManager(GRMainConnection *conn)
{
	this->conn = conn;

	/* No lobby selected yet */
	currentLobby = NULL;
}
/*------------------------------------------------------------------*/
GRLobbyManager::~GRLobbyManager()
{
	/* Cleanup lobbies */
	for(unsigned int x = 0; x < Lobbies.size(); x++) delete(Lobbies[x]);
	Lobbies.clear();
}
/*------------------------------------------------------------------*/
void GRLobbyManager::deleteAll()
{
	for(unsigned int x = 0; x < Lobbies.size(); x++) delete(Lobbies[x]);
	Lobbies.clear();
	GRApplication::getInstance()->getMainWindow()->clearLobbyComboBox();
}
/*------------------------------------------------------------------*/
void GRLobbyManager::addLobby(GRLobby *lobby)
{
	Lobbies.push_back(lobby);

	/* Add to combo box */
	GRApplication::getInstance()->getMainWindow()->addLobbyToComboBox(lobby);
}
/*------------------------------------------------------------------*/
void GRLobbyManager::setCurrentLobby(wxUint32 lobbyID)
{
	GRLobby *lobby = findLobby(lobbyID);
	if(lobby == NULL) return;

	/* Set Current Lobby */
	currentLobby = lobby;

	/* Select combo box */
	GRApplication::getInstance()->getMainWindow()->setCurrentLobbyInComboBox(lobby);
}
/*------------------------------------------------------------------*/
GRLobby *GRLobbyManager::findLobby(wxUint32 lobbyID)
{
	GRLobby *lobby;

	/* if it's offline lobby, obviously it doens't exist */
	if(lobbyID == 0) return NULL;

	for(unsigned int x = 0; x < Lobbies.size(); x++) {
		if(Lobbies[x] == NULL) continue;
		lobby = Lobbies[x];
		if(lobby->lobbyID == lobbyID) return lobby;
	}
	GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Find lobby error: For some odd reason, we couldn't find the lobby we are looking for. Lobby ID: ") + wxString::Format(wxT("%d"), lobbyID));
	return NULL;
}
/*------------------------------------------------------------------*/
GRLobby *GRLobbyManager::getCurrentLobby()
{
	return currentLobby;
}
/*------------------------------------------------------------------*/


