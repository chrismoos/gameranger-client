/*
GameRanger
GRUser.cpp

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

#include "GRUser.h"
#include "GRMainWindow.h"
#include "GRLobby.h"
#include "memdebug.h"

GRUser::GRUser(wxString nick, wxUint32 userID, wxUint32 iconID)
{
	this->nick = nick;
	this->userID = userID;
	this->iconID = iconID;
	icon = NULL;
}
//------------------------------------------------------------
GRUser::GRUser()
{
	this->nick = wxT("default nick");
	this->userID = 0;
	this->iconID = 0;
	icon = NULL;
}
//------------------------------------------------------------
GRUser::~GRUser()
{

}
//------------------------------------------------------------
void GRUser::SetStatus(wxUint8 status)
{
	this->status = status;
}
//-------------------------------------------------------------
bool GRUser::isPremium()
{
	if(status == PREMIUM_IDLE || status == PREMIUM_NOT_IDLE || status == PREMIUM_PLAYING_NOT_IDLE || status == PREMIUM_PLAYING_IDLE)
	{
		return true;
	}
	else return false;
}
//------------------------------------------------------------
bool GRUser::isPlaying()
{
	if(status == PREMIUM_PLAYING_NOT_IDLE || status == PREMIUM_PLAYING_IDLE || status == REGULAR_PLAYING_IDLE || status == REGULAR_PLAYING_NOT_IDLE)
	{
		return true;
	}
	else return false;
}
//---------------------------------------------------------------
bool GRUser::isIdle()
{
	if(status == PREMIUM_PLAYING_IDLE || status == REGULAR_PLAYING_IDLE || status == REGULAR_IDLE || status == PREMIUM_IDLE)
	{
		return true;
	}
	else return false;
}
//----------------------------------------------------------------
void GRUser::addGameToList(GRPlugin *game)
{
	if(game == NULL) return;
	gamesList.push_back(game);
}
//----------------------------------------------------------------
