/*
GameRanger
GRGameParser.cpp

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

#include "GRGameParser.h"
#include "GRPluginManager.h"
#include "GRGameManager.h"
#include "GRUtil.h"
#include "GRGameRoom.h"

GRGameParser::GRGameParser(GRMainConnection *conn)
{
	this->conn = conn;
}
/*------------------------------------------------------------------------------*/
GRGameParser::~GRGameParser()
{

}
/*------------------------------------------------------------------------------*/
bool GRGameParser::run(GR_PACKET *Packet)
{
	switch(Packet->header->command)
	{
		case GAME_ROOMS_LIST:
			parseGameRoomsList(Packet);
		break;

		case GAME_ROOM_OPENED:
			parseGameRoomOpened(Packet);
		break;

		case GAME_ROOM_CLOSED:
			parseGameRoomClosed(Packet);
		break;

		case PLAYER_JOINED_GAME_ROOM:
			parsePlayerJoinedGameRoom(Packet);
		break;

		case PLAYER_LEFT_GAME_ROOM:
			parsePlayerLeftGameRoom(Packet);
		break;

		case GAME_ROOM_STATUS_CHANGED:
			parseGameRoomStatusChanged(Packet);
		break;

		default:
			return false;
		break;
	}

	return true;
}
/*--------------------------------------------------------------------------*/
void GRGameParser::parseGameRoomStatusChanged(GR_PACKET *Packet)
{
	GR_ROOM_STATUS_CHANGED *pck;

	pck = (GR_ROOM_STATUS_CHANGED*)Packet->payload;

	//endian conversion
	pck->roomID = wxUINT32_SWAP_ON_LE(pck->roomID);
	pck->status = wxUINT32_SWAP_ON_LE(pck->status);

	conn->getGameManager()->gameStatusChanged(pck->roomID, pck->status);
}
/*------------------------------------------------------------------------------------*/
void GRGameParser::parsePlayerJoinedGameRoom(GR_PACKET *Packet)
{
	wxUint32 gameRoomID;
	wxUint8 *ptr;
	
	ptr = Packet->payload;

	memcpy(&gameRoomID, ptr, sizeof(wxUint32));
	gameRoomID = ntohl(gameRoomID);

	conn->getGameManager()->incrementRoom(gameRoomID);
}
/*--------------------------------------------------------------------------------*/
void GRGameParser::parsePlayerLeftGameRoom(GR_PACKET *Packet)
{
	wxUint32 gameRoomID;
	wxUint8 *ptr;
	
	ptr = Packet->payload;

	memcpy(&gameRoomID, ptr, sizeof(wxUint32));
	gameRoomID = ntohl(gameRoomID);

	conn->getGameManager()->decrementRoom(gameRoomID);
}
/*--------------------------------------------------------------------------------*/
void GRGameParser::parseGameRoomOpened(GR_PACKET *Packet)
{
	wxUint8 *buf;
	GRGameRoom *gameRoom;
	parseGameReturn *ret;

	buf = Packet->payload;

	ret = parseGame(buf);
	gameRoom = ret->gameRoom;
	delete(ret);

	conn->getGameManager()->addRoomAndNotify(gameRoom);
}
/*---------------------------------------------------------------------------*/
void GRGameParser::parseGameRoomClosed(GR_PACKET *Packet)
{
	wxUint32 roomID;

	memcpy(&roomID, Packet->payload, sizeof(wxUint32));
	roomID = ntohl(roomID);

	conn->getGameManager()->removeRoom(roomID);
}
/*---------------------------------------------------------------------------*/
parseGameReturn *GRGameParser::parseGame(wxUint8 *buf)
{
	GR_GAME_ROOM_PACKET *roomHeader;
	wxString ip, description, nick;
	wxUint32 grID, iconID;
	wxUint8 status, locked;
	GRGameRoom *gameRoom;
	parseGameReturn *ret = new parseGameReturn;

	roomHeader = (GR_GAME_ROOM_PACKET*)buf;

	//endianess
	roomHeader->currentPlayers = ntohl(roomHeader->currentPlayers);
	roomHeader->maxPlayers = ntohl(roomHeader->maxPlayers);
	roomHeader->gameRoomID = ntohl(roomHeader->gameRoomID);
	roomHeader->gameCode = ntohl(roomHeader->gameCode);

	ip = wxString::Format(wxT("%d.%d.%d.%d"), roomHeader->ipAddress[0] ^ 0xFF, roomHeader->ipAddress[1] ^ 0xFF, roomHeader->ipAddress[2] ^ 0xFF, roomHeader->ipAddress[3] ^ 0xFF);

	buf += sizeof(GR_GAME_ROOM_PACKET);

	//locked
	locked = *buf;
	buf++;

	//description
	description = GRUtil::getInstance()->bufToStr(buf);
	buf += description.Len() + 1;

	//gr id
	memcpy(&grID, buf, sizeof(wxUint32));
	grID = ntohl(grID);
	buf += sizeof(wxUint32);

	//icon id
	memcpy(&iconID, buf, sizeof(wxUint32));
	iconID = ntohl(iconID);
	buf += sizeof(wxUint32);

	//status
	status = *buf;
	buf++;

	//nick
	nick = GRUtil::getInstance()->bufToStr(buf);
	buf += nick.Len() + 1;

	//game room list?
	if(*buf == 0) buf += 2;
	else buf += *(buf)+1;

	/* init game room */
	gameRoom = new GRGameRoom(roomHeader->gameRoomID, GRPluginManager::getInstance()->findPluginByCode(roomHeader->gameCode), roomHeader->maxPlayers, roomHeader->currentPlayers,
		ip, locked, description, grID, iconID, status, nick);

	ret->gameRoom = gameRoom;
	ret->ptr = buf;

	return ret;
}
/*-----------------------------------------------------------------------*/
void GRGameParser::parseGameRoomsList(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 gameCount;
	wxUint32 x;
	//GRGameRoom *gameRoom;
	parseGameReturn *ret;
	GRGameManager *gameManager = conn->getGameManager();

	buf = Packet->payload;

	/* game room count */
	memcpy(&gameCount, buf, sizeof(wxUint32));
	gameCount = ntohl(gameCount);
	buf += sizeof(wxUint32);

	/* clear game rooms */
	gameManager->clearAll();

	/* add game rooms */
	for(x = 0; x < gameCount; x++)
	{
		ret = parseGame(buf);
		gameManager->addRoom(ret->gameRoom);
		buf = ret->ptr;
		delete(ret);
	}
}
/*-----------------------------------------------------------------------*/