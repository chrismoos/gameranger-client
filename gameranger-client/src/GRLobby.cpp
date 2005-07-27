/*
GameRanger
GRLobby.cpp

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

#include "GRLobby.h"
#include "GRUser.h"
#include "GRMainWindow.h"
#include "GRLogWindow.h"
#include "GRIcon.h"
#include "GRIconCache.h"
#include "GRGameRoom.h"
#include "memdebug.h"

GRLobby::GRLobby(wxString name, wxUint32 ID, wxUint8 type)
{
	lobbyName = name;
	lobbyID = ID;
	lobbyType = type;
	Users.reserve(400);
	userCount = 0;
}
//-------------------------------------------------------------------
GRLobby::~GRLobby()
{
	wxUint32 x;
	for(x = 0; x < Users.size(); x++)
	{
		delete(Users[x]);
	}
	Users.clear();
}
//--------------------------------------------------------------------
void GRLobby::SetUserCount(wxUint32 count)
{
	userCount = count;
}
//--------------------------------------------------------------------
void GRLobby::AddUser(GRUser *User, int type)
{
	Users.push_back(User);
	if(type == 1) 
	{
		userCount++;
		mainWindow->addTextWithColor(wxT("<< ") + User->nick + wxT(" has joined the room >>\n"), *wxRED);
		updateComboString();
	}
	else if(type == 2) 
	{
		userCount++;
		mainWindow->addTextWithColor(wxT("<< ") + User->nick + wxT(" has joined GameRanger >>\n"), *wxRED);
		updateComboString();
	}
	else 
	{
		userCount++;
		updateComboString();
	}
}
//----------------------------------------------------------------------
void GRLobby::updateComboString()
{
	wxString comboString;
	comboString = lobbyName + wxT("  -  ") + wxString::Format(wxT("%d"), userCount);

	#ifndef __WXGTK__ //wxGTK doesen't have this implemented yet, as of 2.4.2
	mainWindow->lobbyComboBox->SetString(comboIndex, comboString);
	#endif

	mainWindow->lobbyComboBox->SetSelection(mainWindow->currentLobby->comboIndex);
}
//-------------------------------------------------------------------------
void GRLobby::userAction(GR_PACKET *Packet)
{
	wxUint8 *buf;
	GRUser *user;
	wxUint32 prevRoom, newRoom, userID, iconID;
	wxUint8 status;
	GRIcon *icon;
	wxString nick;
	wxInt32 imageIndex;
	wxInt32 index;
	GRLobby *lobby;

	buf = Packet->payload;

	//Previous lobby
	memcpy(&prevRoom, buf, sizeof(wxUint32));
	prevRoom = ntohl(prevRoom);
	buf += sizeof(wxUint32);

	//new lobby
	memcpy(&newRoom, buf, sizeof(wxUint32));
	newRoom = ntohl(newRoom);
	buf += sizeof(wxUint32);

	//User ID
	memcpy(&userID, buf, sizeof(wxUint32));
	userID = ntohl(userID);
	buf += sizeof(wxUint32);

	if(newRoom == lobbyID) //joined this room 
	{ 
			//icon id
			memcpy(&iconID, buf, sizeof(wxUint32));
			iconID = ntohl(iconID);
			buf += sizeof(wxUint32);

			//status
			status = *buf;
			buf++;

			//nickname
			nick = mainWindow->bufToStr(buf);
			buf += nick.Len() + 1;

			user = new GRUser(nick, userID, iconID);

			//games list
			mainWindow->parseGamesListForUser(user, buf);
			buf += *(buf)+1;

			
			user->SetStatus(status);

			if(prevRoom == 0) 
			{
				AddUser(user, 2);
			}
			else 
			{
				//update user count
				lobby = mainWindow->findLobby(prevRoom);
				if(lobby == NULL) return;
				lobby->userCount--;
				lobby->updateComboString();
				AddUser(user, 1);
			}

			icon = mainWindow->iconCache->findIcon(iconID);
			user->icon = icon;
			if(icon == NULL) 
			{
				imageIndex = 0;
				if(iconID != 0) mainWindow->requestIcon(iconID);
			}
			else imageIndex = icon->imageIndex;

			//add to list box
			index = mainWindow->userListBox->InsertItem(mainWindow->userListBox->GetItemCount(), nick, imageIndex);
			if(index != -1) mainWindow->userListBox->SetItemData(index, (long)user);

			return;
	}
	else if(prevRoom == lobbyID) //left room
	{
		if(newRoom == 0) //left gameranger
		{
			RemoveUser(findUser(userID), 2, wxT(""));
		}
		else //went to antoher room
		{
			RemoveUser(findUser(userID), 1, wxT(""));
			lobby = mainWindow->findLobby(newRoom);
			if(lobby == NULL) return;
			lobby->userCount++;
			lobby->updateComboString();
		}
	}
	else //update lobby user counts
	{
		if(prevRoom != 0) 
		{
			lobby = mainWindow->findLobby(prevRoom);
			if(lobby == NULL) return;
			lobby->userCount--;
			lobby->updateComboString();
		}

		if(newRoom != 0)
		{
			lobby = mainWindow->findLobby(newRoom);
			if(lobby == NULL) return;
			lobby->userCount++;
			lobby->updateComboString();
		}
	}

}
//---------------------------------------------------------------------------
GRUser *GRLobby::findUser(wxUint32 userID)
{
	wxUint32 x;
	for(x = 0; x < Users.size(); x++)
	{
		if(Users[x]->userID == userID) return Users[x];
	}
	return NULL;
}
//------------------------------------------------------------------------
void GRLobby::RemoveUser(GRUser *User, int type, wxString reason)
{
	vector<GRUser*>::iterator it;
	wxUint32 x;
	wxInt32 index;
	if(User == NULL) return;

	for(x = 0; x < Users.size(); x++)
	{
		if(Users[x]->userID == User->userID)
		{
			it = Users.begin()+x;
			Users.erase(it);
			break;
		}
	}
	if(type == 1) 
	{
		userCount--;
		mainWindow->chatTextCtrl->AppendText(wxT("<< ") + User->nick + wxT(" has left the room >>\n"));
		updateComboString();
	}
	else if(type == 2) 
	{
		userCount--;
		mainWindow->chatTextCtrl->AppendText(wxT("<< ") + User->nick + wxT(" has left GameRanger >>\n"));
		updateComboString();
	}

	else if(type == 3)
	{
		userCount--;
		mainWindow->chatTextCtrl->AppendText(wxT("<< ") + User->nick + wxT(" was disconnected(") + reason + wxT(") >>\n"));
	}
	index = mainWindow->getUserItemIndex(User);
	if(index == -1) 
	{
		delete(User);
		return;
	}
	mainWindow->userListBox->DeleteItem(index);
	delete(User);

}
//------------------------------------------------------------------------
void GRLobby::userLeftServer(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 userID;
	wxString reason;

	buf = Packet->payload;

	//user id
	memcpy(&userID, buf, sizeof(wxUint32));
	userID = ntohl(userID);
	buf += sizeof(wxUint32);

	//reason
	reason = mainWindow->bufToStr(buf);

	if(reason.Len() > 0) RemoveUser(findUser(userID), 3, reason);
	else RemoveUser(findUser(userID), 2, wxT(""));
}
//-----------------------------------------------------------------------
void GRLobby::chatMessage(GR_PACKET *Packet, int type)
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
	message = mainWindow->bufToStr(buf);

	user = findUser(userID);

	if(type == 0) 
	{
		mainWindow->addTextWithColor(user->nick, *wxBLUE);
		mainWindow->addTextWithColor(wxT(": ") + message + wxT("\n"), *wxBLACK);
	}
	else if(type == 1) 
	{
		mainWindow->addTextWithColor(wxT("*** "),*wxBLACK);
		mainWindow->addTextWithColor(user->nick, *wxBLUE);
		mainWindow->addTextWithColor(wxT(" ") + message + wxT("\n"), *wxBLACK);
	}

	
}
//----------------------------------------------------------------------
void GRLobby::ClearUsers()
{
	int x;
	for(x = 0; x < Users.size(); x++)
	{
		delete(Users[x]);
	}
	Users.clear();
	userCount = 0;
}
//-----------------------------------------------------------------------
void GRLobby::userChangedNick(GR_PACKET *Packet)
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

	user = findUser(userID);
	if(user == NULL) return;

	newNick = mainWindow->bufToStr(ptr);

	mainWindow->chatTextCtrl->AppendText(wxT("<< ")+user->nick+wxT(" is now known as ")+newNick+wxT(">>\n"));
	user->nick = newNick;
	index = mainWindow->getUserItemIndex(user);
	if(index == -1) return;
	mainWindow->userListBox->SetItemText(index, newNick);
}
//---------------------------------------------------------------------
void GRLobby::userIsIdle(GR_PACKET *Packet)
{
	wxUint32 userID;
	wxUint8 *ptr;
	GRUser *user;
	GRGameRoom *gameRoom;

	ptr = Packet->payload;

	memcpy(&userID, ptr, sizeof(wxUint32));
	userID = ntohl(userID);
	
	user = findUser(userID);
	if(user == NULL) return;

	//Incrementing makes them idle
	user->status += 1;

	//update in listbox
	mainWindow->setListInfo(user);

	/* if user is hosting a game, update game idle/not idle */
	for(int x = 0; x < mainWindow->GameRooms.size(); x++) {
		gameRoom = (GRGameRoom*)mainWindow->GameRooms[x];

		/* the user is hosting */
		if(gameRoom->grID == userID) {
			gameRoom->status += 1;
			mainWindow->setGameRoomListInfo(gameRoom);
			break;
		}
	}

	//notify in main window
	//mainWindow->chatTextCtrl->AppendText(wxT("<< ")+user->nick+wxT(" is idle >>\n"));
}
//------------------------------------------------------------------------
void GRLobby::userIsActive(GR_PACKET *Packet)
{
	wxUint32 userID;
	wxUint8 *ptr;
	GRUser *user;
	GRGameRoom *gameRoom;

	ptr = Packet->payload;

	memcpy(&userID, ptr, sizeof(wxUint32));
	userID = ntohl(userID);
	
	user = findUser(userID);
	if(user == NULL) return;

	//Decrementing makes them active
	user->status -= 1;

	//update in listbox
	mainWindow->setListInfo(user);

	/* if user is hosting a game, update game idle/not idle */
	for(int x = 0; x < mainWindow->GameRooms.size(); x++) {
		gameRoom = (GRGameRoom*)mainWindow->GameRooms[x];

		/* the user is hosting */
		if(gameRoom->grID == userID) {
			gameRoom->status -= 1;
			mainWindow->setGameRoomListInfo(gameRoom);
			break;
		}
	}

	//notify in main window
	//mainWindow->chatTextCtrl->AppendText(wxT("<< ")+user->nick+wxT(" is now active >>\n"));
}
//------------------------------------------------------------------------
