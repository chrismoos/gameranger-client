/*
GameRanger
GRLobbyParser.cpp

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

#include "GRLobbyParser.h"
#include "GRApplication.h"
#include "GRLogger.h"
#include "GRIcon.h"
#include "GRLobbyManager.h"
#include "GRLoginWindow.h"
#include "GRConnectStatusWindow.h"
#include "GRUtil.h"

GRLobbyParser::GRLobbyParser(GRMainConnection *conn)
{
	this->conn = conn;
}
/*--------------------------------------------------------------*/
GRLobbyParser::~GRLobbyParser()
{

}
/*--------------------------------------------------------------*/
bool GRLobbyParser::run(GR_PACKET *Packet)
{
	switch(Packet->header->command)
	{
		case PUBLIC_LOBBY_LIST:
			publicLobbyList(Packet);
		break;

		case LOBBY_USER_LIST:
			lobbyUserList(Packet);
		break;

		case LOBBY_USER_ACTION:
			lobbyUserAction(Packet);
		break;

		case USER_LEFT_SERVER:
			userLeftServer(Packet);
		break;

		case LOBBY_CHAT_MESSAGE:
			parseChatMessage(Packet, 0);
		break;	

		case LOBBY_CHAT_MESSAGE_ME:
			parseChatMessage(Packet, 1);
		break;

		case USER_CHANGED_NICKNAME:
			parseUserChangedNick(Packet);
		break;

		case USER_IS_IDLE:
			parseUserIsIdle(Packet);
		break;

		case USER_IS_ACTIVE:
			parseUserIsActive(Packet);
		break;

		default: 
			return false;
		break;
	}

	return true;
}
/*--------------------------------------------------------------*/
void GRLobbyParser::parseUserIsIdle(GR_PACKET *Packet)
{
	wxUint32 userID;
	wxUint8 *ptr;
	GRUser *user;

	ptr = Packet->payload;

	memcpy(&userID, ptr, sizeof(wxUint32));
	userID = ntohl(userID);

	conn->getLobbyManager()->getCurrentLobby()->userIsIdle(userID);
}
/*------------------------------------------------------------------------*/
void GRLobbyParser::parseUserIsActive(GR_PACKET *Packet)
{
	wxUint32 userID;
	wxUint8 *ptr;
	GRUser *user;

	ptr = Packet->payload;

	memcpy(&userID, ptr, sizeof(wxUint32));
	userID = ntohl(userID);

	conn->getLobbyManager()->getCurrentLobby()->userIsActive(userID);
}
/*------------------------------------------------------------------------*/
void GRLobbyParser::parseUserChangedNick(GR_PACKET *Packet)
{
	wxUint32 userID;
	wxString newNick;
	wxInt32 index;
	wxUint8 *ptr;
	GRUser *user;

	ptr = Packet->payload;

	memcpy(&userID, ptr, sizeof(wxUint32));
	userID = ntohl(userID);
	ptr += sizeof(wxUint32);

	newNick = GRUtil::getInstance()->bufToStr(ptr);

	/* Notify current lobby of nick change */
	conn->getLobbyManager()->getCurrentLobby()->userChangedNick(userID, newNick);

	/* TODO - Notify game room manager of nick change */

	/* TODO - Notify private message manager of nick change */
}
/*--------------------------------------------------------------*/
void GRLobbyParser::parseChatMessage(GR_PACKET *Packet, int type)
{
	wxUint8 *buf;
	GRUser *user;
	wxUint32 userID;
	wxString message;

	buf = Packet->payload;

	//user id
	memcpy(&userID, buf, sizeof(wxUint32));
	userID = ntohl(userID);
	buf += sizeof(wxUint32);

	//message
	message = GRUtil::getInstance()->bufToStr(buf);

	conn->getLobbyManager()->getCurrentLobby()->chatMessage(userID, message, type);
}
/*--------------------------------------------------------------*/
void GRLobbyParser::userLeftServer(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 userID;
	wxString reason;
	GRLobby *lobby = conn->getLobbyManager()->getCurrentLobby();

	buf = Packet->payload;

	//user id
	memcpy(&userID, buf, sizeof(wxUint32));
	userID = ntohl(userID);
	buf += sizeof(wxUint32);

	//reason
	reason = GRUtil::getInstance()->bufToStr(buf);

	if(reason.Len() > 0) lobby->removeUserByID(userID, 3, reason);
	else lobby->removeUserByID(userID, 2, wxT(""));
}
/*--------------------------------------------------------------*/
void GRLobbyParser::lobbyUserAction(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 prevRoom, newRoom, userID;
	GRLobby *lobby;
	GRUser *user;

	buf = Packet->payload;

	/* Previous lobby */
	memcpy(&prevRoom, buf, sizeof(wxUint32));
	prevRoom = ntohl(prevRoom);
	buf += sizeof(wxUint32);

	/* new lobby */
	memcpy(&newRoom, buf, sizeof(wxUint32));
	newRoom = ntohl(newRoom);
	buf += sizeof(wxUint32);

	/* User ID */
	memcpy(&userID, buf, sizeof(wxUint32));
	userID = ntohl(userID);
	buf += sizeof(wxUint32);

	if(newRoom == conn->getLobbyManager()->getCurrentLobby()->lobbyID) { /* joined this room  */
		parseUserJoinedLobby(prevRoom, newRoom, userID, buf);
	}
	else if(prevRoom == conn->getLobbyManager()->getCurrentLobby()->lobbyID) {
		if(newRoom == 0) { /* left gameranger */
			conn->getLobbyManager()->getCurrentLobby()->removeUserByID(userID, 2, wxT(""));
		}
		else { /* went to some other room */
			conn->getLobbyManager()->getCurrentLobby()->removeUserByID(userID, 1, wxT(""));
			lobby = conn->getLobbyManager()->findLobby(newRoom);
			if(lobby != NULL) lobby->incrementUserCount();
		}
	}
	else { /* update lobby user counts from other rooms */
		/* old room */
		lobby = conn->getLobbyManager()->findLobby(prevRoom);
		if(lobby != NULL) lobby->decrementUserCount();

		/* new room */
		lobby = conn->getLobbyManager()->findLobby(newRoom);
		if(lobby != NULL) lobby->incrementUserCount();
	}
}
/*--------------------------------------------------------------*/
void GRLobbyParser::parseUserJoinedLobby(wxUint32 previousLobby, wxUint32 newLobby, wxUint32 userID, wxUint8 *buf)
{
	wxUint8 status;
	wxUint32 iconID;
	wxString nick;
	GRLobbyManager *lobbyManager = conn->getLobbyManager();
	GRUser *user;
	GRIcon *icon;

	//icon id
	memcpy(&iconID, buf, sizeof(wxUint32));
	iconID = ntohl(iconID);
	buf += sizeof(wxUint32);

	//status
	status = *buf;
	buf++;

	//nickname
	nick = GRUtil::getInstance()->bufToStr(buf);
	buf += nick.Len() + 1;

	/* make new user */
	user = new GRUser(nick, userID, iconID);

	//games list
	user->parseGamesList(buf);
	buf += *(buf)+1;

	/* set user status */
	user->SetStatus(status);

	/* icon stuff */
	icon = GRIconCache::getInstance()->findIcon(iconID);
	user->icon = icon;
	if(icon == NULL) {
		/* if(iconID != 0) request icon */
	}

	if(previousLobby == 0) /* Joined GameRanger */
	{
		lobbyManager->getCurrentLobby()->addUser(user, 2);
	}
	else /* Joined room from another room */
	{
		lobbyManager->getCurrentLobby()->addUser(user, 1);
		GRLobby *oldLobby = lobbyManager->findLobby(previousLobby);
		if(oldLobby != NULL) {
			oldLobby->decrementUserCount();
		}		
	}
}
/*-------------------------------------------------------------------------------------------*/
void GRLobbyParser::lobbyUserList(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 x;
	wxUint8 status;
	wxUint32 userCount, lobbyID, userID, iconID, imageIndex = 0;
	wxUint32 missingIconCount = 0;
	wxString nick;
	wxInt32 index;
	GRIcon *icon;
	GRUser *user;
	GRLobby *lobby = NULL;

	buf = Packet->payload;

	buf += sizeof(wxUint32); /* some padding */

	/* lobby id for this user list */
	memcpy(&lobbyID, buf, sizeof(wxUint32));
	lobbyID = ntohl(lobbyID);
	buf += sizeof(wxUint32);

	/* get current lobby */
	lobby = conn->getLobbyManager()->findLobby(lobbyID);
	if(lobby == NULL) {
		GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Fatal error: Where the fuck is the lobby for this user list?"));
		return;
	}

	/* select current lobby */
	conn->getLobbyManager()->setCurrentLobby(lobbyID);

	/* remove all users from existing user list box */
	GRApplication::getInstance()->getMainWindow()->clearUserListBox();

	/* remove all users from the lobby */
	lobby->clearAll();

	/* Tell user we are in the room */
	GRApplication::getInstance()->getMainWindow()->addTextWithColor(wxT("______")+lobby->lobbyName+wxT("______\n"), *wxRED);

	//user count
	memcpy(&userCount, buf, sizeof(wxUint32));
	userCount = ntohl(userCount);
	buf += sizeof(wxUint32)*2;

	for(x = 0; x < userCount; x++) //for each user
	{
		//user id
		memcpy(&userID, buf, sizeof(wxUint32));
		userID = ntohl(userID);
		buf += sizeof(wxUint32);

		//icon id
		memcpy(&iconID, buf, sizeof(wxUint32));
		iconID = ntohl(iconID);
		buf += sizeof(wxUint32);

		//status
		status = *buf;
		buf++;

		//nickname 
		nick = GRUtil::getInstance()->bufToStr(buf);
		buf += nick.Len() + 1;

		//make user
		user = new GRUser(nick, userID, iconID);

		//game list?
		user->parseGamesList(buf);
		buf += *(buf)+1;

		user->SetStatus(status);

		/* Add user to lobby */
		icon = GRIconCache::getInstance()->findIcon(iconID);
		user->icon = icon;
		if(icon == NULL) {
			if(iconID != 0) missingIconCount++;
		}
		lobby->addUser(user, 0);
	}
	
	requestMissingIconsFromUserList(missingIconCount);
}
/*--------------------------------------------------------------*/
void GRLobbyParser::requestMissingIconsFromUserList(wxUint32 count)
{
	wxUint32 missingCount = 0, len = 0, pos = 0, x;
	wxUint32 iconID;
	wxUint8 *buf;
	GRLobby *currentLobby = conn->getLobbyManager()->getCurrentLobby();

	//If we don't need any icons, no point in sending it
	if(count == 0) return;

	/* If current lobby is null, we can't do this */
	if(currentLobby == NULL) return;

	len = sizeof(wxUint32) + (sizeof(wxUint32)*count);

	buf = new wxUint8[len];

	missingCount = count;
	missingCount = htonl(missingCount);

	memcpy(buf, (wxUint8*)&missingCount, sizeof(wxUint32));
	pos += sizeof(wxUint32);

	for(x = 0; x < currentLobby->Users.size(); x++)
	{
		if(currentLobby->Users[x]->icon == NULL && currentLobby->Users[x]->iconID != 0) //we don't have this icon
		{	
			iconID = currentLobby->Users[x]->iconID;
			iconID = htonl(iconID);
			memcpy(buf+pos, (wxUint8*)&iconID, sizeof(wxUint32));
			pos += sizeof(wxUint32);
		}
	}

	GRApplication::getInstance()->getMainConnection()->sendGRPacket(REQUEST_ICON_DATA, len, buf);
	delete[] buf;
}
//------------------------------------------------------------------------------
void GRLobbyParser::publicLobbyList(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 x;
	wxUint32 lobbyCount, lobbyID, users;
	wxUint8 type;
	wxString lobbyName;
	wxString comboString;
	GRLobby *lobby;

	buf = Packet->payload;

	memcpy(&lobbyCount, buf, sizeof(wxUint32));
	lobbyCount = ntohl(lobbyCount);
	buf += sizeof(wxUint32);

	/* Clear old lobbies */
	conn->getLobbyManager()->deleteAll();

	for(x = 0; x < lobbyCount; x++)
	{
		//lobby id
		memcpy(&lobbyID, buf, sizeof(wxUint32));
		lobbyID = ntohl(lobbyID);
		buf += sizeof(wxUint32);

		//users 
		memcpy(&users, buf, sizeof(wxUint32));
		users = ntohl(users);
		buf += sizeof(wxUint32);

		//type
		type = *buf;
		buf++;

		//lobby name
		lobbyName = GRUtil::getInstance()->bufToStr(buf);
		buf += lobbyName.Len() + 1;

		lobby = new GRLobby(lobbyName, lobbyID, type);
		lobby->SetUserCount(users);

		conn->getLobbyManager()->addLobby(lobby);	
	}

	GRLoginWindow::getInstance()->Show(false);
	GRConnectStatusWindow::getInstance(GRApplication::getInstance()->getMainWindow())->Show(false);
}
/*--------------------------------------------------------------*/
