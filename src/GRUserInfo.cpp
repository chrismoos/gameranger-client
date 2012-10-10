/*
GameRanger
GRUserInfo.cpp

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

#include "GRUserInfo.h"
#include "memdebug.h"

GRUserInfo::GRUserInfo()
{
	m_nickname = wxT("default nick");
	m_realName = wxT("default name");
	m_icon = NULL;
}
//-----------------------------------------------------------------------
GRUserInfo::~GRUserInfo()
{

}
//------------------------------------------------------------------------
GRUserInfo::GRUserInfo(wxString nickname, wxString realName, wxUint32 userID, wxUint32 lastLogin,
		wxUint32 lastLogout, wxUint32 idleSince, wxUint32 iconID, wxUint32 roomID, wxUint8 *location, GRLobby *lobby, GRIcon *icon, wxString email)
{
	m_nickname = nickname;
	m_realName = realName;
	m_userID = userID;
	m_lastLogin = lastLogin;
	m_lastLogout = lastLogout;
	m_idleSince = idleSince;
	m_iconID = iconID;
	m_roomID = roomID;
	m_lobby = lobby;
	m_email = email;
	m_icon = icon;
	memcpy(m_location, location, 12);
}
//---------------------------------------------------------------------------
