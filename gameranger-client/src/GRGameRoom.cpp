/*
GameRanger
GRGameRoom.cpp

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

#include "GRGameRoom.h"
#include "GRPlugin.h"
#include "memdebug.h"

GRGameRoom::GRGameRoom(wxUint32 gameRoomID, GRPlugin *Plugin, wxUint32 maxPlayers, wxUint32 currentPlayers, wxString ip, wxUint8 locked, wxString description, wxUint32 grID, wxUint32 iconID, wxUint32 status, wxString host)
{
	this->gameRoomID = gameRoomID;
	this->Plugin = Plugin;
	this->currentPlayers = currentPlayers;
	this->maxPlayers = maxPlayers;
	this->grID = grID;
	this->host = host;
	this->description = description;
	this->iconID = iconID;
	this->ip = ip;
	this->locked = (wxUint8)locked;
	this->status = (wxUint32)status;
}
//------------------------------------------------------------------
GRGameRoom::~GRGameRoom()
{

}
//---------------------------------------------------------------------
bool GRGameRoom::isPremium()
{
	if(status == PREMIUM_IDLE || status == PREMIUM_NOT_IDLE || status == PREMIUM_PLAYING_NOT_IDLE || status == PREMIUM_PLAYING_IDLE)
	{
		return true;
	}
	else return false;
}
//------------------------------------------------------------
bool GRGameRoom::isPlaying()
{
	if(status == PREMIUM_PLAYING_NOT_IDLE || status == PREMIUM_PLAYING_IDLE || status == REGULAR_PLAYING_IDLE || status == REGULAR_PLAYING_NOT_IDLE)
	{
		return true;
	}
	else return false;
}
//---------------------------------------------------------------
bool GRGameRoom::isIdle()
{
	if(status == PREMIUM_PLAYING_IDLE || status == REGULAR_PLAYING_IDLE || status == REGULAR_IDLE || status == PREMIUM_IDLE)
	{
		return true;
	}
	else return false;
}
//----------------------------------------------------------------