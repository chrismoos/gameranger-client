/*
GameRanger
GRUserInfo.h

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

#ifndef GR_USER_INFO_H
#define GR_USER_INFO_H

#include "GRBaseDefs.h"
#include "GRIcon.h"
#include "GRLobby.h"

class GRUserInfo
{
public:
	//default constructor
	GRUserInfo();
	GRUserInfo(wxString nickname, wxString realName, wxUint32 userID, wxUint32 lastLogin,
		wxUint32 lastLogout, wxUint32 idleSince, wxUint32 iconID, wxUint32 roomID, wxUint8 *location, GRLobby *lobby, GRIcon *icon, wxString email);
	//overloading constructor

	//user info stuff
	wxString m_nickname;
	wxString m_realName;
	wxString m_email;
	wxUint32 m_userID;
	wxUint32 m_lastLogin;
	wxUint32 m_lastLogout;
	wxUint32 m_idleSince;
	wxUint32 m_iconID;
	wxUint32 m_roomID;
	wxUint8 m_location[12];
	GRLobby *m_lobby;
	GRIcon *m_icon;
	

	~GRUserInfo();
};

#endif
