/*
GameRanger
GRGameRoomWindow.cpp

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

#ifdef WIN32
#include <windows.h>
#endif
#include "GRGameRoomWindow.h"
#include "GRMainWindow.h"
#include "GRGameRoom.h"
#include "GRUser.h"
#include "GRIconCache.h"
#include "GRIcon.h"
#include "GRLogWindow.h"
#include "GRPrivateMessage.h"
#include "memdebug.h"

GRGameRoomWindow::GRGameRoomWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *)parent, -1, title, pos, size)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create Controls
	createControls();

	//Center window
	CentreOnScreen();

	gameRoom = 0;
	active = true;
}
//-----------------------------------------------------------------------------------
GRGameRoomWindow::~GRGameRoomWindow()
{
	wxUint32 x;
	vector <GRGameRoomWindow*>::iterator it;
	GRGameRoomWindow *window;
	if(active) gameRoom->currentPlayers--;
	if(mainWindow == NULL) return;
	mainWindow->updateGameRoomPlayerCountString(gameRoom);

	//Delete this instance from main window's pm array
	for(x = 0; x < mainWindow->gameRoomWindows.size(); x++)
	{
		window = mainWindow->gameRoomWindows[x];
		if(window == this) //found it
		{
			it = mainWindow->gameRoomWindows.begin()+x;
			mainWindow->gameRoomWindows.erase(it);
			break;
		}
	}

	for(x = 0; x < users.size(); x++) delete(users[x]);
	users.clear();
	mainWindow->currentGameRoom = NULL;
	if(active) mainWindow->sendGRPacket(LEAVE_GAME_ROOM, 0, NULL);
	if(actionButton->GetLabel() == wxT("Abort")) {
		mainWindow->sendGRPacket(ABORT_GAME_ROOM, 0, NULL);
		mainWindow->gameRoomWillClose = true;
	}

	mainWindow->currentRoomID = 0;
	
}
//--------------------------------------------------------------------------------
void GRGameRoomWindow::createControls()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);

	//Add chat edit field
	chatEditField = new wxTextCtrl(this, GAMEROOM_CHATEDIT_ID, wxT(""), wxDefaultPosition,
		wxSize(475, 20), wxTE_PROCESS_ENTER);

	//Add chat window
	chatWindow = new wxTextCtrl(this, GAMEROOM_CHATLOG_ID, wxT(""), wxDefaultPosition,
		wxSize(475, 350), wxTE_MULTILINE | wxTE_NOHIDESEL | wxTE_READONLY | wxTE_RICH2 | wxTE_AUTO_URL);

	//Add user list box
	userListBox = new wxListCtrl(this, GAMEROOM_USERLIST_ID, wxDefaultPosition,
		wxSize(180, 400), wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER | wxLC_ALIGN_LEFT);
	userListBox->InsertColumn(0, wxT("nick"), wxLIST_FORMAT_LEFT, 175);

	/* Add join/host/abort button */
	actionButton = new wxButton(this, GAMEROOM_ACTION_BUTTON_ID, wxT("Null"), 
		wxDefaultPosition, wxDefaultSize);

	//game list menu
	gameListMenu = new wxMenu();

	//User list box popup menu
	popupMenu = new wxMenu();
    popupMenu->Append(GAMEROOM_USER_GET_INFO, wxT("Get &user info"));
	popupMenu->Append(GAMEROOM_LIST_MENU, wxT("Games"), gameListMenu);


	chatEditField->SetFocus();

	wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
	leftSizer->Add(chatWindow, 1, wxALL, 0);
	leftSizer->Add(chatEditField, 0, wxTOP, 5);

	/* right sizer */
	rightSizer->Add(userListBox, 1, wxALL | wxGROW, 0);
	rightSizer->Add(actionButton, 0, wxTOP, 5);

	//Add stuff to sizer
	sizer->Add(leftSizer, 0, wxALL | wxGROW, 5);
	sizer->Add(rightSizer, 0, wxRIGHT | wxTOP | wxBOTTOM | wxGROW, 5);
	//sizer->Add(userListBox, 0, wxRIGHT | wxTOP | wxBOTTOM | wxGROW, 5);

	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(sizer);
	sizer->Fit(this);
	sizer->SetSizeHints(this);
}
//------------------------------------------------------------------------------------
void GRGameRoomWindow::AddUser(GRUser *user, bool notify)
{
	GRIcon *icon;
	wxInt32 imageIndex, index;

	users.push_back(user);

	icon = mainWindow->iconCache->findIcon(user->iconID);
	user->icon = icon;
	if(icon == NULL) 
	{
		imageIndex = 0;
		if(user->iconID != 0) mainWindow->requestIcon(user->iconID);
	}
	else imageIndex = icon->imageIndex;

	//add to list box
	index = userListBox->InsertItem(userListBox->GetItemCount(), user->nick, imageIndex);
	if(index != -1) userListBox->SetItemData(index, (long)user);
	setListInfo(user);

	if(notify == true) 
	{
		addTextWithColor(wxT("<< ") + user->nick + wxT(" has joined the room >>\n"), *wxRED);
	}
}
//----------------------------------------------------------------------------------
void GRGameRoomWindow::addTextWithColor(wxString str, wxColour color)
{
	chatWindow->SetDefaultStyle(wxTextAttr(color, *wxWHITE));
	chatWindow->AppendText(str);
	chatWindow->SetDefaultStyle(*wxRED);

	chatWindow->SetInsertionPointEnd();

}
//-----------------------------------------------------------------------------
void GRGameRoomWindow::setListInfo(GRUser *User)
{
	wxListItem info;
	wxInt32 index;
	wxColor color;
	wxFont font;

	index = getUserItemIndex(User);
	if(index == -1) 
	{
		return;
	}

	info.SetId(index);
	userListBox->GetItem(info);

	if(User->isPremium()) font.SetWeight(wxBOLD);
	if(User->isIdle()) info.SetTextColour(*wxLIGHT_GREY);

	info.SetFont(font);
	userListBox->SetItem(info);

}
//---------------------------------------------------------------------------
wxInt32 GRGameRoomWindow::getUserItemIndex(GRUser *user)
{
	wxInt32 x;
	GRUser *temp;
	for(x = 0; x < userListBox->GetItemCount(); x++)
	{
		temp = (GRUser*)userListBox->GetItemData(x);
		if(user == temp) return x;
	}
	mainWindow->logWindow->logText(wxString::Format(wxT("---Error: Couldnt' find list item for User ID: %d"), user->userID));
	return -1;
}
//-----------------------------------------------------------------------------
void GRGameRoomWindow::userJoined(GR_PACKET *Packet)
{
	wxUint8 *ptr;
	wxUint32 userID, iconID;
	GRUser *user;
	wxUint8 status;
	wxString nickname;

	ptr = Packet->payload;

	//user id
	memcpy(&userID, ptr, sizeof(wxUint32));
	userID = wxUINT32_SWAP_ON_LE(userID);
	ptr += sizeof(wxUint32);

	//icon id
	memcpy(&iconID, ptr, sizeof(wxUint32));
	iconID = wxUINT32_SWAP_ON_LE(iconID);
	ptr += sizeof(wxUint32);

	//status
	status = *ptr;
	ptr++;

	//nickname
	nickname = mainWindow->bufToStr(ptr);
	ptr += nickname.Len() + 1;

	user = new GRUser(nickname, userID, iconID);

	//game list
	mainWindow->parseGamesListForUser(user, ptr);
	ptr += *(ptr)+1;

	
	user->SetStatus(status);
	AddUser(user, true);
	gameRoom->currentPlayers++;
	mainWindow->updateGameRoomPlayerCountString(gameRoom);
}
//-----------------------------------------------------------------------------
void GRGameRoomWindow::chatMessage(GR_PACKET *Packet, int type)
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
		addTextWithColor(user->nick, *wxBLUE);
		addTextWithColor(wxT(": ") + message + wxT("\n"), *wxBLACK);
	}
	else if(type == 1) 
	{
		addTextWithColor(wxT("*** "),*wxBLACK);
		addTextWithColor(user->nick, *wxBLUE);
		addTextWithColor(wxT(" ") + message + wxT("\n"), *wxBLACK);
	}
}
//------------------------------------------------------------------------------
GRUser *GRGameRoomWindow::findUser(wxUint32 userID)
{
	wxUint32 x;
	for(x = 0; x < users.size(); x++)
	{
		if(users[x]->userID == userID) return users[x];
	}
	return NULL;
}
//------------------------------------------------------------------------
void GRGameRoomWindow::OnChatEditEnter(wxCommandEvent &event)
{
	if(chatEditField->GetValue().Len() == 0) return;
	if(mainWindow->currentProfile == NULL) return;
	mainWindow->sendGRPacket(SEND_GAME_ROOM_MESSAGE, chatEditField->GetValue().Len()+1, (wxUint8*)(const char*)chatEditField->GetValue().mb_str());

	addTextWithColor(mainWindow->myNickname, *wxRED);
	addTextWithColor(wxT(": ") + chatEditField->GetValue() + wxT("\n"), *wxBLACK);

	chatEditField->SetValue(wxT(""));
}
//-----------------------------------------------------------------------------
void GRGameRoomWindow::userLeft(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 userID;
	wxString reason;

	buf = Packet->payload;

	//user id
	memcpy(&userID, buf, sizeof(wxUint32));
	userID = ntohl(userID);
	buf += sizeof(wxUint32);
	
	RemoveUser(findUser(userID));
	gameRoom->currentPlayers--;
	mainWindow->updateGameRoomPlayerCountString(gameRoom);
}
//-----------------------------------------------------------------------
void GRGameRoomWindow::RemoveUser(GRUser *User)
{
	vector<GRUser*>::iterator it;
	wxUint32 x;
	wxInt32 index;
	if(User == NULL) return;

	for(x = 0; x < users.size(); x++)
	{
		if(users[x]->userID == User->userID)
		{
			it = users.begin()+x;
			users.erase(it);
			break;
		}
	}

	if(User->userID == mainWindow->myUserID) {
		addTextWithColor(wxT("<< You have been kicked from the room >>"), *wxRED);
		userListBox->ClearAll();
		mainWindow->currentGameRoom = NULL;
		active = false;
	}
	else {
		addTextWithColor(wxT("<< ") + User->nick + wxT(" has left the room >>\n"), *wxRED);
	}

	index = getUserItemIndex(User);
	if(index == -1) 
	{
		delete(User);
		return;
	}
	userListBox->DeleteItem(index);
	delete(User);

}
//------------------------------------------------------------------------
void GRGameRoomWindow::OnUserDoubleClick(wxListEvent& event)
{
	int index;
	GRUser *user;
	GRPrivateMessage *msg;
	
	index = event.GetIndex();

	if(index == -1) return;

	
	user = (GRUser*)userListBox->GetItemData(event.GetIndex());
	if(user == NULL) return;

	//See if message window is already open, and if so, give it focus
	msg = mainWindow->findPrivateMessageByID(user->userID);
	if(msg != NULL) //it's open, give it focus
	{
		msg->SetFocus();
		return;
	}

	//Create new PM Window
	GRPrivateMessage *pm = new GRPrivateMessage(this, wxT("Private Message - ")+user->nick, wxDefaultPosition, wxDefaultSize);
	pm->mainWindow = mainWindow;
	pm->userID = user->userID;
	pm->nickname = user->nick;
	pm->Show(true);
    mainWindow->privateMessages.push_back(pm);
}
//--------------------------------------------------------------------------
void GRGameRoomWindow::deleteUsers()
{
	userListBox->DeleteAllItems();
	for(int x = 0; x < users.size(); x++) delete(users[x]);
	users.clear();
}
//-------------------------------------------------------------------------------
void GRGameRoomWindow::OnButtonClick(wxCommandEvent &event)
{
	wxString actionMsg = actionButton->GetLabel();
	
	/* start game(hosts only) */
	if(actionMsg == "Start") {
		mainWindow->sendGRPacket(GAME_LAUNCH_LOADING, 0, NULL);
		mainWindow->sendGRPacket(GAME_LAUNCH_DONE, 0, NULL);
		this->gameRoom->status += 8;
		mainWindow->setGameRoomListInfo(gameRoom);
		actionButton->SetLabel(wxT("Abort"));
		addTextWithColor(wxT("<< Starting game >>\n"), *wxRED);
#ifdef WIN32
		ShellExecute(NULL, "Open", 
			"C:\\Program Files\\Red Storm Entertainment\\Rogue Spear\\UrbanOperations.exe", 
			"-server 2346", 
			"C:\\Program Files\\Red Storm Entertainment\\Rogue Spear", SW_NORMAL);
#endif
	}
	else if(actionMsg == wxT("Abort")) { /* abort game(hosts and players) */
		mainWindow->sendGRPacket(ABORT_GAME_ROOM, 0, NULL);
		addTextWithColor(wxT("<< Aborting game >>\n"), *wxRED);
		if(this->gameRoom->grID == mainWindow->myUserID) {
			actionButton->SetLabel(wxT("Start"));
		}
		else {
			actionButton->SetLabel(wxT("Join"));
		}
	}
	else if(actionMsg == wxT("Join")) { /* players only */
		actionButton->SetLabel(wxT("Abort"));
		mainWindow->sendGRPacket(GAME_LAUNCH_LOADING, 0, NULL);
		mainWindow->sendGRPacket(GAME_LAUNCH_DONE, 0, NULL);
#ifdef WIN32
		ShellExecute(NULL, "Open", 
			"C:\\Program Files\\Red Storm Entertainment\\Rogue Spear\\UrbanOperations.exe", 
			wxString::Format("-client %s 2346", gameRoom->ip), 
			"C:\\Program Files\\Red Storm Entertainment\\Rogue Spear", SW_NORMAL);
#endif
	}
}
//--------------------------------------------------------------------------------
void GRGameRoomWindow::OnUserRightClick(wxListEvent &event)
{
	wxPoint pt;
	if(event.GetIndex() == -1) return;

	pt = event.GetPoint();

	//make games list menu
	clearGamesListMenu();
	mainWindow->makeGameListMenu(userListBox, gameListMenu, event.GetIndex());

	userListBox->PopupMenu(popupMenu, pt);
}
//----------------------------------------------------------------------------------
void GRGameRoomWindow::clearGamesListMenu()
{
	popupMenu->Destroy(GAMEROOM_LIST_MENU);
	gameListMenu = new wxMenu();
	popupMenu->Append(GAMEROOM_LIST_MENU, wxT("Games"), gameListMenu);
}
//------------------------------------------------------------------------------------
void GRGameRoomWindow::OnUserListGetInfo(wxCommandEvent &event)
{
	int index = -1;
	wxUint32 userID;
	GRUser *user;

	index = userListBox->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if(index == -1) return;


	user = (GRUser*)userListBox->GetItemData(index);
	if(user == NULL) return;

	userID = user->userID;

	userID = wxUINT32_SWAP_ON_LE(userID);
	mainWindow->roomWantedInfo = true;
	mainWindow->sendGRPacket(GET_USER_INFO, sizeof(wxUint32), (wxUint8*)&userID);
}
//----------------------------------------------------------------------------------
