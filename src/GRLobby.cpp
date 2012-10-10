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
#include "GRApplication.h"
#include "GRLobbyManager.h"
#include "GRLogger.h"
#include "GRMainWindow.h"
#include "GRLogWindow.h"
#include "GRIcon.h"
#include "GRIconCache.h"
#include "GRGameRoom.h"
#include "memdebug.h"
#include "GRProfile.h"

GRLobby::GRLobby(wxString name, wxUint32 ID, wxUint8 type)
{
	lobbyName = name;
	lobbyID = ID;
	lobbyType = type;
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
void GRLobby::addUser(GRUser *User, int type)
{
	switch(type) 
	{
		case 1:
			GRApplication::getInstance()->getMainConnection()->requestIcon(User->iconID);
			GRApplication::getInstance()->getMainWindow()->displayUserJoinedRoom(User);
		break;

		case 2:
			GRApplication::getInstance()->getMainConnection()->requestIcon(User->iconID);
			GRApplication::getInstance()->getMainWindow()->displayUserJoinedGR(User);
		break;
	}

	/* Add user to our user list */
	Users.push_back(User);
	userCount++;

	/* Update lobby combo box string */
	GRApplication::getInstance()->getMainWindow()->updateLobbyInComboBox(this);

	/* Add user to list box */
	GRApplication::getInstance()->getMainWindow()->addUserToListBox(User);
}
/*----------------------------------------------------------------------*/
void GRLobby::decrementUserCount()
{
	userCount--;

	/* Update lobby combo box string */
	GRApplication::getInstance()->getMainWindow()->updateLobbyInComboBox(this);
}
/*----------------------------------------------------------------------*/
void GRLobby::incrementUserCount()
{
	userCount++;

	/* Update lobby combo box string */
	GRApplication::getInstance()->getMainWindow()->updateLobbyInComboBox(this);
}
/*----------------------------------------------------------------------*/
void GRLobby::joinLobby()
{
	wxUint32 id;

	/* if it's our current lobby, return */
	if(GRApplication::getInstance()->getMainConnection()->getLobbyManager()->getCurrentLobby() == this) return;

	id = this->lobbyID;
	id = htonl(id);
	GRApplication::getInstance()->getMainConnection()->sendGRPacket(JOIN_PUBLIC_LOBBY, sizeof(wxUint32), (wxUint8*)&id);
}
/*----------------------------------------------------------------------*/
void GRLobby::removeUserByID(wxUint32 userID, int type, wxString reason)
{
	GRUser *user = findUser(userID);
	if(user == NULL) return;
	removeUser(user, type, reason);
}
/*-------------------------------------------------------------------------*/
void GRLobby::removeUser(GRUser *user, int type, wxString reason)
{
	vector<GRUser*>::iterator it;

	switch(type)
	{
		case 1:
			GRApplication::getInstance()->getMainWindow()->displayUserLeftRoom(user);
		break;

		case 2:
			GRApplication::getInstance()->getMainWindow()->displayUserLeftGR(user);
		break;

		case 3:
			GRApplication::getInstance()->getMainWindow()->displayUserLeftGR(user, reason);
		break;
	}

	/* Remove user from list */
	for(unsigned int x = 0; x < Users.size(); x++) {
		if(Users[x]->userID == user->userID) {
			it = Users.begin()+x;
			Users.erase(it);
			break;
		}
	}

	/* Remove user from list box */
	GRApplication::getInstance()->getMainWindow()->removeUserFromListBox(user);

	/* Update combo box */
	userCount--;
	GRApplication::getInstance()->getMainWindow()->updateLobbyInComboBox(this);

	/* Delete user */
	delete(user);
}
/*-------------------------------------------------------------------------*/
void GRLobby::purgeIcons()
{
	wxUint32 x;
	wxInt32 index;
	GRUser *user;
	GRIconCache *iconCache = GRIconCache::getInstance();
	GRMainWindow *mainWindow = GRApplication::getInstance()->getMainWindow();

	/* update in lobby */
	for(x = 0; x < Users.size(); x++)
	{
		user = Users[x];
		if(user == NULL) continue;
		if(user->icon == NULL && user->iconID != 0) {
			user->icon = iconCache->findIcon(user->iconID);
			if(user->icon != NULL) {
				mainWindow->updateUserIcon(user);
			}
		}
	}
}
/*-------------------------------------------------------------------------*/
void GRLobby::AddUser(GRUser *User, int type)
{

}
/*-------------------------------------------------------------------------*/
void GRLobby::updateComboString()
{
	GRApplication::getInstance()->getMainWindow()->setCurrentLobbyInComboBox(this);
}
/*-------------------------------------------------------------------------*/
void GRLobby::clearAll()
{
	wxUint32 x;
	for(x = 0; x < Users.size(); x++)
	{
		delete(Users[x]);
	}
	Users.clear();
	userCount = 0;
}
/*-------------------------------------------------------------------------*/
void GRLobby::userAction(GR_PACKET *Packet)
{

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


}
//-----------------------------------------------------------------------
void GRLobby::chatMessage(wxUint32 userID, wxString message, int type)
{
	GRUser *user = findUser(userID);
	if(user != NULL) chatMessage(user, message, type);
}
//----------------------------------------------------------------------
void GRLobby::chatMessage(GRUser *user, wxString Message, int type)
{
	GRMainWindow *mainWindow = GRApplication::getInstance()->getMainWindow();
	if(type == 0) 
	{
		mainWindow->addTextWithColor(user->nick, *wxBLUE);
		mainWindow->addTextWithColor(wxT(": ") + Message + wxT("\n"), *wxBLACK);
	}
	else if(type == 1) 
	{
		mainWindow->addTextWithColor(wxT("*** "),*wxBLACK);
		mainWindow->addTextWithColor(user->nick, *wxBLUE);
		mainWindow->addTextWithColor(wxT(" ") + Message + wxT("\n"), *wxBLACK);
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
void GRLobby::userChangedNick(wxUint32 userID, wxString newNick)
{
	GRUser *user = findUser(userID);
	if(user != NULL) userChangedNick(user, newNick);
}
/*-----------------------------------------------------------------------*/
void GRLobby::userChangedNick(GRUser *user, wxString newNick)
{
	GRMainWindow *mainWindow = GRApplication::getInstance()->getMainWindow();
	GRProfile *profile = GRApplication::getInstance()->getMainConnection()->getProfile();

	/* notify in main chat */
	mainWindow->addTextWithColor(wxT("<< ")+user->nick+wxT(" is now known as ")+newNick+wxT(">>\n"), *wxRED);

	/* update user */
	user->nick = newNick;

	/* update user list box */
	mainWindow->updateUserInListBox(user);

	/* if it's our user, update profile too */
	if(user->userID == profile->grID) {
		profile->nickname = newNick;
	}
}
/*-----------------------------------------------------------------------*/
void GRLobby::userIsIdle(GRUser *user)
{
	/* Incrementing makes them idle */
	user->setIdle();

	/* update in listbox */
	GRApplication::getInstance()->getMainWindow()->setListInfo(user);

	/* notify in main window */
	GRApplication::getInstance()->getMainWindow()->addTextWithColor(wxT("<< ")+user->nick+wxT(" is idle >>\n"), *wxRED);
}
/*-----------------------------------------------------------------------*/
void GRLobby::userIsIdle(wxUint32 userID)
{
	GRUser *user = findUser(userID);
	if(user != NULL) userIsIdle(user);
}
/*-----------------------------------------------------------------------*/
void GRLobby::userIsActive(GRUser *user)
{
	/* Incrementing makes them idle */
	user->setActive();

	/* update in listbox */
	GRApplication::getInstance()->getMainWindow()->setListInfo(user);

	/* notify in main window */
	GRApplication::getInstance()->getMainWindow()->addTextWithColor(wxT("<< ")+user->nick+wxT(" is active >>\n"), *wxRED);
}
/*-----------------------------------------------------------------------*/
void GRLobby::userIsActive(wxUint32 userID)
{
	GRUser *user = findUser(userID);
	if(user != NULL) userIsActive(user);
}
/*-----------------------------------------------------------------------*/
void GRLobby::userChangedIcon(wxUint32 userID, wxUint32 iconID)
{
	GRUser *user = findUser(userID);
	GRIcon *icon;

	if(user == NULL) return; /* we don't care */

	user->iconID = iconID;

	user->icon = GRIconCache::getInstance()->findIcon(iconID);
	if(user->icon != NULL) {
		GRApplication::getInstance()->getMainWindow()->updateUserIcon(user);
	}
}
/*-----------------------------------------------------------------------*/
