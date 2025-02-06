/*
GameRanger
GRMainWindow.cpp

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

#include <wx/mstream.h>
#include <algorithm>
#include "GRMainWindow.h"
#include "GRLoginWindow.h"
#include "GRApplication.h"
#include "GRLobbyManager.h"
#include "GRIcon.h"
#include "GRUser.h"
#include "GRLogger.h"
#include "GRLogWindow.h"
#include "GRSecurity.h"
#include "GRHostWindow.h"
#include "GRConnectStatusWindow.h"
#include "GRLobby.h"
#include "GRUser.h"
#include "GRPlugin.h"
#include "GRIcon.h"
#include "GRIconCache.h"
#include "GRProfile.h"
#include "GRGameRoom.h"
#include "GRRegWindow.h"
#include "GRPrivateMessage.h"
#include "GRPremiumUserInfoWindow.h"
#include "GRGameRoomWindow.h"
#include "GRUserInfo.h"
#include "GRUserInfoWindow.h"
#include "GRPremiumUserInfo.h"
#include "GRChangeIconWindow.h"
#include "GRFindPlayerWindow.h"
#include "GRChangeMyGames.h"
#include "memdebug.h"

#ifdef _DEBUG 
#define LOG_PACKETS
#endif


GRMainWindow::GRMainWindow(const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) NULL, GRMAINWINDOW_ID, title, pos, size)
{
	/* Get logger */
	logger = GRLogger::getInstance();

	/* Get plugin manager */
	pluginManager = GRPluginManager::getInstance();

	/* image handlers */
	wxInitAllImageHandlers();

	/* setup image lists */
	setupImageLists();

	/* Setup window */
	setupWindow();
	createControls();
	CentreOnScreen();

	/* disable menus */
	disableMenus();
/*



	
	packetCounter = 0;

	currentProfile = 0;
	currentGameRoom = NULL;
	currentPremiumInfoWindow = NULL;
	searchWindow = NULL;

	
	*/
}
/*--------------------------------------------------------------------------------------*/
void GRMainWindow::setupIconImageList()
{
	GRIconCache *icnCache = GRIconCache::getInstance();
	iconImgList = new wxImageList(16, 16);
	icnCache->imgList = iconImgList;
	for(size_t x = 0; x < icnCache->Icons.size(); x++) {
		icnCache->Icons[x]->imageIndex = iconImgList->Add(*icnCache->Icons[x]->image);
	}
	userListBox->AssignImageList(iconImgList, wxIMAGE_LIST_SMALL);
}
/*--------------------------------------------------------------------------------------*/
void GRMainWindow::setupImageLists()
{
	wxString path = wxGetCwd();

	/* Image List */
	imgList = new wxImageList(16, 16);

	/* Icons for games list */

	/* players icon */
	if(wxFile::Exists(wxT("images/players.xpm"))) {
		wxBitmap pic(path+wxT("/images/players.xpm"), wxBITMAP_TYPE_XPM);
		imgList->Add(pic);
	}

	/* lock icon */
	if(wxFile::Exists(wxT("images/lock.xpm"))) {
		wxBitmap pic2(path+wxT("/images/lock.xpm"), wxBITMAP_TYPE_XPM);
		imgList->Add(pic2);
	}

	/* go icon */
	if(wxFile::Exists(wxT("images/go.xpm"))) {
		wxBitmap pic1(path+wxT("/images/go.xpm"), wxBITMAP_TYPE_XPM);
		imgList->Add(pic1);
	}
	
	/* Icon Image List */
	if(wxFile::Exists(wxT("images/defaulticon.xpm"))) {
		iconImgList = new wxImageList(16, 16);
		wxBitmap pic3(path+wxT("/images/defaulticon.xpm"), wxBITMAP_TYPE_XPM);
		iconImgList->Add(pic3);
	}

	if(imgList->GetImageCount() == 0) {
		GRLogger::getInstance()->log(GRLogger::LOG_WARNING, wxT("The default images could not be loaded."));
	}
	else {
		GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Default images successfully loaded."));
	}
}
/*--------------------------------------------------------------------------------------*/
void GRMainWindow::setupWindow()
{
	//window specific stuff	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create Menu Bar
	fileMenu = new wxMenu();
	fileMenu->Append(FILE_MENU_EXIT, wxT("E&xit"));
	fileMenu->Append(FILE_MENU_LOGOUT, wxT("&Logout"));
	fileMenu->Enable(FILE_MENU_LOGOUT, false);

	//Find Player Menu
	usersMenu = new wxMenu();
	usersMenu->Append(FIND_PLAYER_MENU, wxT("&Find Player"));
	
	//Accounts Bar
	accountsMenu = new wxMenu();
	accountsMenu->Append(REGISTER_MENU_ITEM, wxT("&Register"));
	accountsMenu->Append(LOGIN_MENU_ITEM, wxT("&Login"));

	//game list menu
	gameListMenu = new wxMenu();

	//User list box popup menu
	popupMenu = new wxMenu();
	popupMenu->Append(USER_LIST_MENU_GET_INFO, wxT("Get &user info"));
	popupMenu->Append(GAMES_LIST_MENU, wxT("Games"), gameListMenu);

	//Options Menu
	optionsMenu = new wxMenu();
	optionsMenu->Append(CHANGE_NICK_MENU_ITEM, wxT("Change &nickname"));
	optionsMenu->Append(CHANGE_NAME_MENU_ITEM, wxT("Change &real name"));
	optionsMenu->Append(CHANGE_PASSWORD_MENU_ITEM, wxT("Change &password"));
	optionsMenu->Append(CHANGE_ICON_MENU_ITEM, wxT("Change &icon"));
	optionsMenu->Append(CHANGE_MY_GAMES_ITEM, wxT("Change My &Games"));
	timestampItem = optionsMenu->AppendCheckItem(TIMESTAMP_TOGGLE, wxT("Display timestamps"));

	aboutMenu = new wxMenu();
	aboutMenu->Append(ABOUT_MENU_ABOUT, wxT("&About"));

	hostMenu = new wxMenu();
	hostMenu->Append(HOST_MENU_ITEM, wxT("Host"));

	menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(accountsMenu, wxT("&Accounts"));
	menuBar->Append(hostMenu, wxT("&Game Rooms"));
	menuBar->Append(optionsMenu, wxT("&Options"));
	menuBar->Append(usersMenu, wxT("&Users"));
	menuBar->Append(aboutMenu, wxT("&Help"));

	SetMenuBar(menuBar);
}
/*--------------------------------------------------------------------------------------*/
void GRMainWindow::OnWindowClose(wxCloseEvent &event)
{
	logger->log(GRLogger::LOG_INFO, wxT("Main window has been successfully closed."));
	GRApplication::getInstance()->mainWindowHasClosed();
	GRApplication::getInstance()->closeMainConnection();
	this->Destroy();
}
/*--------------------------------------------------------------------------------------*/
void GRMainWindow::enableMenus()
{
	hostMenu->Enable(HOST_MENU_ITEM, true);
	optionsMenu->Enable(CHANGE_NICK_MENU_ITEM, true);
	optionsMenu->Enable(CHANGE_NAME_MENU_ITEM, true);
	optionsMenu->Enable(CHANGE_PASSWORD_MENU_ITEM, true);
	optionsMenu->Enable(CHANGE_ICON_MENU_ITEM, true);
	optionsMenu->Enable(CHANGE_MY_GAMES_ITEM, true);
	usersMenu->Enable(FIND_PLAYER_MENU, true);
	fileMenu->Enable(FILE_MENU_LOGOUT, true);
}
/*--------------------------------------------------------------------------------------*/
void GRMainWindow::disableMenus()
{
	hostMenu->Enable(HOST_MENU_ITEM, false);
	optionsMenu->Enable(CHANGE_NICK_MENU_ITEM, false);
	optionsMenu->Enable(CHANGE_NAME_MENU_ITEM, false);
	optionsMenu->Enable(CHANGE_PASSWORD_MENU_ITEM, false);
	optionsMenu->Enable(CHANGE_ICON_MENU_ITEM, false);
	optionsMenu->Enable(CHANGE_MY_GAMES_ITEM, false);
	usersMenu->Enable(FIND_PLAYER_MENU, false);
	fileMenu->Enable(FILE_MENU_LOGOUT, false);
}
/*--------------------------------------------------------------------------------------*/
GRMainWindow::~GRMainWindow()
{
	wxUint32 x;

	/* delete popup menu */
	if(popupMenu != NULL)	delete(popupMenu);

	/* cleanup image lists */
	iconImgList = NULL;
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::clearLobbyComboBox()
{
	lobbyComboBox->Clear();
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::addLobbyToComboBox(GRLobby *lobby)
{
	wxString comboString;
	
	comboString = lobby->lobbyName + wxT("  -  ") + wxString::Format(wxT("%d"), lobby->userCount);
	lobbyComboBox->Append(comboString, (void*)lobby);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::setCurrentLobbyInComboBox(GRLobby *lobby)
{
	int index;
	wxString comboString;

	index = getLobbyComboIndex(lobby);
	if(index == -1) return;

	comboString = lobby->lobbyName + wxT("  -  ") + wxString::Format(wxT("%d"), lobby->userCount);

	lobbyComboBox->SetString(index, comboString);
	lobbyComboBox->SetSelection(index);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::updateLobbyInComboBox(GRLobby *lobby)
{
	int index;
	wxString comboString;

	index = getLobbyComboIndex(lobby);
	if(index == -1) return;

	comboString = lobby->lobbyName + wxT("  -  ") + wxString::Format(wxT("%d"), lobby->userCount);

	lobbyComboBox->SetString(index, comboString);

	/* select current lobby */
	index = getLobbyComboIndex(GRApplication::getInstance()->getMainConnection()->getLobbyManager()->getCurrentLobby());
	if(index != -1)	lobbyComboBox->SetSelection(index);
}
/*------------------------------------------------------------------------------------*/
int GRMainWindow::getLobbyComboIndex(GRLobby *lobby)
{
	GRLobby *temp;
	for(unsigned int x = 0; x < lobbyComboBox->GetCount(); x++) {
		temp = (GRLobby*)lobbyComboBox->GetClientData(x);
		if(temp == NULL) continue;
		if(temp->lobbyID == lobby->lobbyID) return x;
	}
	GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Find lobby combo box index error: For some odd reason, we couldn't find the lobby's item in the combo box. Lobby ID: ") + wxString::Format(wxT("%d"), lobby->lobbyID));
	return -1;
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::clearUserListBox()
{
	userListBox->DeleteAllItems();
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::displayUserJoinedRoom(GRUser *user)
{
	addTextWithColor(wxT("<< ") + user->nick + wxT(" has joined the room >>\n"), *wxRED);
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::displayUserJoinedGR(GRUser *user)
{
	addTextWithColor(wxT("<< ") + user->nick + wxT(" has joined GameRanger >>\n"), *wxRED);
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::displayUserLeftRoom(GRUser *user)
{
	addTextWithColor(wxT("<< ") + user->nick + wxT(" has left the room >>\n"), *wxRED);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::displayUserLeftGR(GRUser *user)
{
	addTextWithColor(wxT("<< ") + user->nick + wxT(" has left GameRanger >>\n"), *wxRED);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::displayUserLeftGR(GRUser *user, wxString reason)
{
	addTextWithColor(wxT("<< ") + user->nick + wxT(" was disconnected(") + reason + wxT(") >>\n"), *wxRED);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::displayGameRoomOpened(GRGameRoom *gameRoom)
{
	addTextWithColor(wxT("<< ") + gameRoom->host + wxT(" has started a ") + gameRoom->Plugin->gameName + wxT(" room >>\n"), *wxRED);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::displayGameRoomClosed(GRGameRoom *gameRoom)
{
	addTextWithColor(wxT("<< ") + gameRoom->host + wxT(" has closed a ") + gameRoom->Plugin->gameName + wxT(" room >>\n"), *wxRED);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::updateUserIcon(GRUser *user)
{
	int index = getUserListBoxIndex(user);
	if(index != -1 && user->icon != NULL) userListBox->SetItemImage(index, user->icon->imageIndex, user->icon->imageIndex);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::updateUserInListBox(GRUser *user)
{
	int index = getUserListBoxIndex(user);
	if(index != -1) userListBox->SetItemText(index, user->nick);
}
/*------------------------------------------------------------------------------------*/
void GRMainWindow::addUserToListBox(GRUser *user)
{
	int index;
	index = userListBox->InsertItem(userListBox->GetItemCount(), user->nick);
	if(index != -1) {
		userListBox->SetItemData(index, (long)user);
		if(user->icon != NULL) userListBox->SetItemImage(index, user->icon->imageIndex);
		else userListBox->SetItemImage(index, 0);
	}
	setListInfo(user);
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::removeUserFromListBox(GRUser *user)
{
	int index;
	index = getUserListBoxIndex(user);
	if(index != -1) userListBox->DeleteItem(index);
}
/*-------------------------------------------------------------------------------------*/
int GRMainWindow::getUserListBoxIndex(GRUser *user)
{
	for(size_t x = 0; x < userListBox->GetItemCount(); x++) {
		if(userListBox->GetItemData(x) == (unsigned long)user) return x;
	}
	return -1;
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::clearGameRooms()
{
	gameRoomList->DeleteAllItems();
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::addGameRoomToListView(GRGameRoom *gameRoom)
{
	int index;
	wxString str;
	GRPlugin *plugin;

	if(gameRoom == NULL) return;

	plugin = gameRoom->Plugin;

	/* add subitems and main item */
	index = gameRoomList->InsertItem(gameRoomList->GetItemCount(), plugin->gameName, plugin->imageIndex);
	if(index == -1) return;
	gameRoomList->SetItem(index, 1, gameRoom->host, -1);
	gameRoomList->SetItem(index, 2, gameRoom->description);
	gameRoomList->SetItem(index, 3, gameRoom->ip);
	gameRoomList->SetItem(index, 4, wxString::Format(wxT("%d/%d"), gameRoom->currentPlayers, gameRoom->maxPlayers));

	/* Set item data so we can find it */
	gameRoomList->SetItemData(index, (long)gameRoom);

	
	/* set game room list info */
	setGameRoomListInfo(gameRoom);
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::updateGameRoomInListView(GRGameRoom *gameRoom)
{
	int index;

	if(gameRoom == NULL) return;

	index = getGameItemIndex(gameRoom);
	if(index == -1) return;

	gameRoomList->SetItem(index, 1, gameRoom->host, -1);
	gameRoomList->SetItem(index, 2, gameRoom->description);
	gameRoomList->SetItem(index, 3, gameRoom->ip);
	gameRoomList->SetItem(index, 4, wxString::Format(wxT("%d/%d"), gameRoom->currentPlayers, gameRoom->maxPlayers));

	setGameRoomListInfo(gameRoom);
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::removeGameFromListView(GRGameRoom *gameRoom)
{
	wxInt32 index = getGameItemIndex(gameRoom);
	if(index != -1) gameRoomList->DeleteItem(index);
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::OnLoadCacheDone(wxCommandEvent &evt)
{
	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Icon cache successfully loaded."));
	setupIconImageList();
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::OnPluginLoadDone(wxCommandEvent &evt)
{
	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Plugins successfully loaded."));
	setupPluginImageList();
}
/*-------------------------------------------------------------------------------------*/
void GRMainWindow::setupPluginImageList()
{
	GRPluginManager *pluginManager = GRPluginManager::getInstance();
	GRPlugin *plugin;

	for(size_t x = 0; x < pluginManager->getPluginCount(); x++) {
		plugin = pluginManager->getPluginAt(x);
		if(plugin == NULL) continue;
		if(plugin->image == NULL) continue;
		pluginManager->getPluginAt(x)->imageIndex = imgList->Add(*pluginManager->getPluginAt(x)->image);
	}
}
/*--------------------------------------------------------------------------------------*/
void GRMainWindow::cleanupGR()
{
	int x;
	for(x = 0; x < (int)Lobbies.size(); x++)
	{
		delete(Lobbies[x]);
	}
	Lobbies.clear();
	for(x = 0; x < (int)GameRooms.size(); x++)
	{
		delete(GameRooms[x]);
	}
	GameRooms.clear();
	for(x = 0; x < (int)privateMessages.size(); x++)
	{
		delete(privateMessages[x]);
	}
	privateMessages.clear();
	for(x = 0; x < (int)gameRoomWindows.size(); x++)
	{
		delete(gameRoomWindows[x]);
	}
	gameRoomWindows.clear();
}
//------------------------------------------------------------------------------------
void GRMainWindow::Login(char *email, char *password)
{
	//this->Show(true);
	strcpy(GRemail, email);
	strcpy(GRpassword, password);
	connectToServer();

/*	testPacket(wxT("data/0000000F.bin"));
	testPacket(wxT("data/00000013.bin"));
	testPacket(wxT("data/000000BC.bin"));
	testPacket(wxT("data/00000019.bin"));
	testPacket(wxT("data/0000001A.bin"));
	testPacket(wxT("data/0000000A.bin"));
	testPacket(wxT("data/00000038.bin"));
	testPacket(wxT("data/0000005F.bin"));
	testPacket(wxT("data/0000001F.bin"));*/
	//testPacket(wxT("data/000000D1.bin"));
	//testPacket(wxT("data/000000D3.bin"));
	//testPacket(wxT("data/0000000B.bin"));
	
}
//-------------------------------------------------------------------------------------
void GRMainWindow::createControls()
{
	//Main Sizer and Panel
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);


	gameRoomList = new wxListView(this, GAMELIST_ID, wxDefaultPosition,
		wxSize(510, 215), wxLC_REPORT | wxLC_SORT_ASCENDING | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT);
	
	gameRoomList->AssignImageList(imgList, wxIMAGE_LIST_SMALL);
	
	gameRoomList->InsertColumn(0, wxT("Game Name"), wxLIST_FORMAT_LEFT, 175);
	gameRoomList->InsertColumn(1, wxT("Host"), wxLIST_FORMAT_LEFT, 155);
	gameRoomList->InsertColumn(2, wxT("Description"), wxLIST_FORMAT_LEFT, 250);
	gameRoomList->InsertColumn(3, wxT("IP Address"), wxLIST_FORMAT_LEFT, 100);
	gameRoomList->InsertColumn(4, wxT(""), wxLIST_FORMAT_LEFT, 50);
	gameRoomList->SetColumnImage(4, 0);
	gameRoomList->InsertColumn(5, wxT(""), wxLIST_FORMAT_LEFT, 34);
	gameRoomList->SetColumnImage(5, 2);
	gameRoomList->InsertColumn(6, wxT(""), wxLIST_FORMAT_LEFT, 34);
	gameRoomList->SetColumnImage(6, 1);

	
	//Add Notebook to sizer
	//topSizer->Add(notebook, 2, wxALIGN_TOP | wxALIGN_CENTER | wxALL | wxGROW | wxEXPAND, 5);
	topSizer->Add(gameRoomList, 2, wxALIGN_TOP | wxALIGN_CENTER | wxALL, 5);
	

	//ComboBox
	lobbyComboBox = new wxComboBox(this, COMBOBOX_ID, wxT(""), wxDefaultPosition,
		wxSize(175, 25), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	

	//User list/combo box sizer
	wxBoxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);

	//Main Chat Window
	chatTextCtrl = new wxTextCtrl(this, CHATLOG_ID, wxT(""), wxDefaultPosition,
		wxSize(600, 300), wxTE_MULTILINE | wxTE_NOHIDESEL | wxTE_READONLY | wxTE_RICH2 | wxTE_AUTO_URL);
	//set attributes for chat window
	chatTextCtrl->SetDefaultStyle(wxTextAttr(*wxRED, *wxWHITE));
	chatTextCtrl->SetValue(wxT(""));

	chatEditField = new wxTextCtrl(this, CHATEDIT_ID, wxT(""), wxDefaultPosition,
		wxSize(50, 20), wxTE_PROCESS_ENTER);

	//User list listbox
	userListBox = new wxListCtrl(this, USERLISTBOX_ID, wxDefaultPosition,
		wxSize(200, 300), wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING  | wxLC_NO_HEADER | wxLC_ALIGN_LEFT);

	userListBox->InsertColumn(0, wxT("nick"), wxLIST_FORMAT_LEFT, 175);

	//Chat Sizer and middle sizer
	wxBoxSizer *chatSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *middleSizer = new wxBoxSizer(wxVERTICAL);

	//Chat window and chat edit field
	middleSizer->Add(chatTextCtrl, 1, wxALIGN_CENTER | wxRIGHT | wxBOTTOM, 5);
	middleSizer->Add(chatEditField, 0, wxALIGN_CENTER | wxRIGHT, 5);

	//user list/combo box
	rightSizer->Add(lobbyComboBox, 0, wxALIGN_RIGHT | wxALIGN_TOP | wxRIGHT | wxBOTTOM, 5);
	rightSizer->Add(userListBox, 1, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);

	//add to main chat sizer
	chatSizer->Add(middleSizer, 3, wxALIGN_CENTER | wxALL, 0);
	chatSizer->Add(rightSizer, 1, wxALL, 0);
	
	topSizer->Add(chatSizer, 3, wxBOTTOM | wxLEFT | wxRIGHT, 5);


	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(topSizer);
	topSizer->Fit(this);
	topSizer->SetSizeHints(this);
}
//--------------------------------------------------------------------------------
void GRMainWindow::MenuExit(wxCommandEvent &event)
{
	this->Close();
}
//----------------------------------------------------------------------------------
void GRMainWindow::OnSocketEvent(wxSocketEvent& event)
{
	switch(event.GetSocketEvent()) 
	{
		case wxSOCKET_CONNECTION:
			OnConnect();
		break;

		case wxSOCKET_LOST:
			if(connecting == true) OnUnableToConnect();
			else if(connecting == false) OnDisconnect();
		break;

		case wxSOCKET_INPUT:
			OnDataAvailable();
		break;
	}
}
//------------------------------------------------------------------------------------
void GRMainWindow::OnConnect()
{
	connecting = false;
	statusWindow->SetTitle(wxT("Connected to GameRanger server..."));
	statusWindow->statusIndicator->SetValue(2);
	loggedIn = false;
	packetCounter = 0;
	currentRoomID = 0;
	roomWantedInfo = false;
	showTimestamps = false;
	gameRoomWillClose = false;
	fileMenu->Enable(FILE_MENU_LOGOUT, true);
	timer->Start(360000, false);
}
//----------------------------------------------------------------------------------
void GRMainWindow::OnUnableToConnect()
{
	connecting = false;
	statusWindow->SetTitle(wxT("Disconnected from GameRanger server."));
	statusWindow->statusIndicator->SetValue(0);
	wxMessageDialog *msgDialog = new wxMessageDialog(statusWindow, wxT("Unable to connect to the GameRanger server. Please check your connection and try again."), wxT("Connection error"), wxICON_ERROR);
	msgDialog->ShowModal();
	delete(msgDialog);
	statusWindow->Show(false);
}
//-------------------------------------------------------------------------------
void GRMainWindow::OnDisconnect()
{
	addTextWithColor(wxT("<< Disconnected from GameRanger >>\n"), *wxRED);

	userListBox->DeleteAllItems();
	gameRoomList->DeleteAllItems();
	lobbyComboBox->Clear();
	disableMenus();
}
//-----------------------------------------------------------------------------
void GRMainWindow::OnDataAvailable()
{
	GR_PACKET_HEADER pckHeader;
	GR_PACKET Packet;
	wxUint32 pos = 0;
	wxUint8 *payload;

	//make sure we haven't deleted socket(Socket->Close() is delayed)
	if(m_socket == NULL) return;

	//read in header
	m_socket->Read(&pckHeader, sizeof(pckHeader));

	//endian conversion
	pckHeader.command = ntohl(pckHeader.command);
	pckHeader.payloadLength = ntohl(pckHeader.payloadLength);

	//read in payload
	payload = new wxUint8[pckHeader.payloadLength];

	pos = 0;
	for(;;)
	{
		m_socket->Read(payload+pos, pckHeader.payloadLength-pos);
		pos += m_socket->LastCount();
		if(pos == pckHeader.payloadLength) break;
	}

	if(m_socket->Error()) 
	{
		logWindow->logText(wxT("socket error"));
	}

	//logWindow->logText(wxString::Format(wxT("read: %d bytes"), pos));

	//put everything into our packet structure
	Packet.header = &pckHeader;
	Packet.payload = payload;

	

	logWindow->logText(wxString::Format(wxT("Command: %08X\nPayload Length: %d"), pckHeader.command, pckHeader.payloadLength));
	
	logWindow->logData(Packet.payload, Packet.header->payloadLength);

#ifdef LOG_PACKETS
	wxUint8 *raw = createRawPacket(pckHeader.command, pckHeader.payloadLength, payload);
	logPacket(raw, pckHeader.payloadLength+sizeof(GR_PACKET_HEADER));
	delete[] raw;
#endif


	handlePacket(&Packet);

	delete[] payload;
}
//----------------------------------------------------------------------------
void GRMainWindow::handlePacket(GR_PACKET *Packet)
{
	wxMessageDialog *msgDlg;

	//Big ugly List of Commands
	switch(Packet->header->command)
	{
		case GAME_ROOM_USER_LIST:
			gameRoomUserList(Packet);
		break;

		case PLAYER_JOINED_MY_GAME_ROOM:
			if(currentGameRoom != NULL) currentGameRoom->userJoined(Packet);
		break;

		case PLAYER_LEFT_MY_GAME_ROOM:
			if(currentGameRoom != NULL) currentGameRoom->userLeft(Packet);
		break;

		case GAME_ROOM_CHAT_MESSAGE:
			if(currentGameRoom != NULL) currentGameRoom->chatMessage(Packet, 0);
		break;

		case APP_BANNER:
			appBanner(Packet);
		break;

		case GAME_ROOM_LAUNCHED:
			gameRoomLaunched(Packet);
		break;

		default:
			if(logWindow != NULL)
			{
				logWindow->logText(wxString::Format(wxT("Unknown command: %08X\n"), Packet->header->command));
			}
		break;
	}
}
//------------------------------------------------------------------------------
void GRMainWindow::gameRoomLaunched(GR_PACKET *Packet)
{
	wxString pass = bufToStr(Packet->payload);

	if(currentGameRoom == NULL) /* we aren't in a room */
		return;

	currentGameRoom->gameRoom->status += 8;
	setGameRoomListInfo(currentGameRoom->gameRoom);

	/* if we are hosting, change button to abort */
	if(currentGameRoom->gameRoom->grID == myUserID) {
		currentGameRoom->actionButton->SetLabel(wxT("Abort"));
	}
	else { /* enable join button */
		currentGameRoom->actionButton->Enable(true);
	}

	/* now all packets from GR have halted, once we return, it will send us game room list, lobby list, etc,. */
	currentGameRoom->addTextWithColor(wxT("<< Game Started >>\n"), *wxRED);
	currentGameRoom->addTextWithColor(wxT("<< Game Room Password: ")+pass+wxT(" >>\n"), *wxRED);
}
void GRMainWindow::sendGRPacket(wxUint32 command, wxUint32 payloadLength, wxUint8 *payload)
{
	GR_PACKET_HEADER header;
	wxUint32 len = 0;
	wxUint8 *packet;
	wxUint8 counterByte;
	bool writeCounterByte = false;

	header.command = htonl(command);

	writeCounterByte = writeCounter(command);
	
	if(writeCounterByte == false) header.payloadLength = htonl(payloadLength);
	else header.payloadLength = htonl(payloadLength+1);	
	

	//Check if we should append packet counter byte
	if(writeCounterByte == false) len = sizeof(header)+payloadLength;
	else len = sizeof(header)+payloadLength + sizeof(wxUint8);


	packet = new wxUint8[len];

	memcpy(packet, &header, sizeof(header));
	memcpy(packet+sizeof(header), payload, payloadLength);

	//append packet counter byte
	if(writeCounterByte == true) 
	{
		counterByte = getPacketCounterByte(command, payloadLength);
		memcpy(packet+sizeof(header)+payloadLength, (wxUint8*)&counterByte, sizeof(wxUint8));
	}
		

	logWindow->logData(packet, len);

	if(m_socket->IsConnected())
		m_socket->Write(packet, len);

	delete[] packet;
}
//--------------------------------------------------------------------------------
void GRMainWindow::connectToServer()
{
	wxIPV4address addr;

	//fill address struct in
	addr.Hostname(wxT(SERVER_HOSTNAME));
	addr.Service(SERVER_PORT);

	//open connecting window
	statusWindow->Show(true);
	statusWindow->SetTitle(wxT("Connecting to GameRanger server..."));
	statusWindow->statusIndicator->SetRange(6);
	statusWindow->statusIndicator->SetValue(1);
	statusWindow->SetFocus();
	//statusWindow->MakeModal();


	//connect, and don't block
	connecting = true;
	if(m_socket == NULL) 
	{
		m_socket = new wxSocketClient();
		m_socket->SetEventHandler(*this, SOCKET_ID1);
		m_socket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_CONNECTION_FLAG | wxSOCKET_LOST_FLAG);
		m_socket->Notify(true);
	}
	m_socket->SetFlags(wxSOCKET_WAITALL);
	m_socket->Connect(addr, false);

}
//----------------------------------------------------------------------------
wxString GRMainWindow::bufToStr(wxUint8 *text)
{
	wxUint32 x;
	wxString szNew;

	for(x = 0;;x++) {
		if(text[x] == 0) break;
		szNew += wxString::Format(wxT("%c"), text[x]);
	}

	return szNew;
}
//-------------------------------------------------------------------------
GRLobby *GRMainWindow::findLobby(wxUint32 lobbyID)
{
	wxUint32 x;
	for(x = 0; x < Lobbies.size(); x++)
	{
		if(Lobbies[x]->lobbyID == lobbyID) return Lobbies[x];
	}
	return NULL;
}
//------------------------------------------------------------------------
void GRMainWindow::addGameRoom(wxUint32 gameRoomID, wxUint32 gameCode, wxUint32 maxPlayers, wxUint32 currentPlayers, wxString ip, wxUint8 locked, wxString description, wxUint32 grID, wxUint32 iconID, wxUint32 status, wxString host, bool announce)
{
	int index;
	GRGameRoom *room;
	GRPlugin *plugin;
	wxString str;
	plugin = pluginManager->findPluginByCode(gameCode);
	index = gameRoomList->InsertItem(userListBox->GetItemCount(), plugin->gameName, plugin->imageIndex);
	gameRoomList->SetItem(index, 1, host, -1);
	gameRoomList->SetItem(index, 2, description);
	gameRoomList->SetItem(index, 3, ip);
	gameRoomList->SetItem(index, 4, wxString::Format(wxT("%d/%d"), currentPlayers, maxPlayers));
	if(status > 3) 
	{
		#ifdef WIN32 //windows won't show subitem images correctly, so just display text
		gameRoomList->SetItem(index, 5, wxT("Yes"), -1);
		#else
		gameRoomList->SetItem(index, 5, wxT(""), 2);
		#endif
	}
	else  
	{
		gameRoomList->SetItem(index, 5, wxT(""), -1);
	}

	room = new GRGameRoom(gameRoomID, plugin, maxPlayers, currentPlayers, ip, locked, description, grID, iconID, status, host);

	logWindow->logText(wxString::Format(wxT("Locked: %d\n"), room->locked));
	//check if it's locked
	if(room->isLocked())
	{
		#ifdef WIN32 //windows won't show subitem images correctly, so just display text
		gameRoomList->SetItem(index, 6, wxT("Yes"), -1);
		#else
		gameRoomList->SetItem(index, 6, wxT(""), 1);
		#endif
	}


	gameRoomList->SetItemData(index, (long)room);
	GameRooms.push_back(room);

	//update list info with some bold/idle stuff
	setGameRoomListInfo(room);

	if(announce == true) 
	{	
		str = wxT("<< ") + room->host + wxT(" has started a ") + room->Plugin->gameName + wxT(" room >>");
		addTextWithColor(str+wxT("\n"), *wxRED);
	}

	/* if we are hosting it and it doesn't already exist, open window accordingly */
	if(room->grID == this->myUserID) {
		if(currentGameRoom != NULL) {
			currentGameRoom->SetFocus();
			currentGameRoom->gameRoom = room;
			return;
		}
		GRGameRoomWindow *gameRoom = new GRGameRoomWindow(this, room->Plugin->gameName+wxT(" - ")+room->host, wxDefaultPosition, wxDefaultSize);
		wxIcon roomIcon;

		gameRoom->mainWindow = this;
		gameRoom->gameRoom = room;
	

		if(room->Plugin->image != NULL) 
		{
			roomIcon.CopyFromBitmap(wxBitmap(*room->Plugin->image));
		}

		gameRoom->SetIcon(roomIcon);
		gameRoom->userListBox->SetImageList(iconImgList, wxIMAGE_LIST_SMALL);
		gameRoom->addTextWithColor(wxT("Description: ")+room->description+wxT("\n\n"), *wxRED);
		gameRoomWindows.push_back(gameRoom);
		currentGameRoom = gameRoom;

		GRUser *user = new GRUser(room->host, room->grID, room->iconID);
		user->SetStatus(status);
		currentGameRoom->AddUser(user, false);
		currentGameRoom->actionButton->SetLabel(wxT("Start"));

		setGameRoomListInfo(currentGameRoom->gameRoom);
		updateGameRoomPlayerCountString(currentGameRoom->gameRoom);
		gameRoom->Show(true);
	}
	if(currentGameRoom != NULL) {
		if(room->gameRoomID == currentRoomID) {
			currentGameRoom->SetFocus();
			currentGameRoom->gameRoom = room;
		}
	}
}
//-----------------------------------------------------------------------------
void GRMainWindow::setListInfo(GRUser *User)
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
void GRMainWindow::OnComboBoxSelect(wxCommandEvent &event)
{
	wxInt32 index;
	GRLobby *lobby;

	index = lobbyComboBox->GetSelection();
	
	if(index == -1) return;
	
	lobby = (GRLobby*)lobbyComboBox->GetClientData(index);
	if(lobby != NULL) lobby->joinLobby();
}
//--------------------------------------------------------------------------
void GRMainWindow::joinLobby(GRLobby *lobby)
{
	wxUint32 id;
	id = lobby->lobbyID;
	id = htonl(id);
	sendGRPacket(JOIN_PUBLIC_LOBBY, sizeof(wxUint32), (wxUint8*)&id);
}
//-------------------------------------------------------------------------
wxUint32 GRMainWindow::getPacketCounterByte(wxUint32 command, wxUint32 payloadLength)
{
	wxUint32 r0;

	if(packetCounter == 0) 
	{
		lastCommand = command;
		lastLength = payloadLength;
		packetCounter++;
		return 0;
	}

	r0 = lastCommand;
	r0 ^= lastLength;
	r0 ^= packetCounter;
	r0 &= 0x000000FF;

	lastCommand = command;
	lastLength = payloadLength;
	

	logWindow->logText(wxString::Format(wxT("packet counter: %08X, final: %08X\n"), packetCounter, r0));

	packetCounter++;
	return r0;
}
//-------------------------------------------------------------------------------
void GRMainWindow::OnTimer(wxTimerEvent &evt)
{
	logWindow->logText(wxT("Wrote pulse."));
	sendGRPacket(GR_ALIVE_PULSE, 0, NULL);
}
//------------------------------------------------------------------------------
bool GRMainWindow::writeCounter(wxUint32 command)
{
	switch(command)
	{
		case CLIENT_VERIFY_ACK:
			return false;
		break;

		case CREATE_ACCOUNT:
			return false;
		break;

		case CONFIRM_EMAIL_ADDRESS:
			return false;
		break;

		case LOGIN_TO_GAMERANGER:
			return false;
		break;

		case GR_ALIVE_PULSE:
			return false;
		break;

		default:
			return true;
		break;
	}
}
//-----------------------------------------------------------------------------
void GRMainWindow::OnChatEditEnter(wxCommandEvent &event)
{
	GRUser *user;

	if(chatEditField->GetValue().Len() == 0) return;
	user = GRApplication::getInstance()->getMainConnection()->getLobbyManager()->getCurrentLobby()->findUser(GRApplication::getInstance()->getMainConnection()->getProfile()->grID);
	if(user == NULL) return;
		
	GRApplication::getInstance()->getMainConnection()->sendGRPacket(SEND_LOBBY_CHAT_MESSAGE, chatEditField->GetValue().Len()+1, (wxUint8*)(const char*)chatEditField->GetValue().mb_str());

	addTextWithColor(user->nick, *wxRED);
	addTextWithColor(wxT(": ") + chatEditField->GetValue() + wxT("\n"), *wxBLACK);

	chatEditField->SetValue(wxT(""));
}
//-----------------------------------------------------------------------------
void GRMainWindow::OnLoginMenu(wxCommandEvent &event)
{
	GRLoginWindow *loginWindow = GRLoginWindow::getInstance(this);
	loginWindow->populateProfilesList();
	loginWindow->Show(true);
	loginWindow->SetFocus();
}
//---------------------------------------------------------------------------
void GRMainWindow::OnRegisterMenu(wxCommandEvent &event)
{
	//there is already a registration wizard open
	if(regWindow != NULL) return;

	//Run reg wizard
	regWindow = new GRRegWindow(wxT("reg"), wxDefaultPosition, wxSize(0,0));
	regWindow->mainWindow = this;
	regWindow->Show(false);
}
//--------------------------------------------------------------------------------
void GRMainWindow::OnAboutMenu(wxCommandEvent &event)
{
	wxMessageDialog *msgDlg = new wxMessageDialog(this, 
		wxT("This application was created by xoclipse. It is released under the GPL. For more information, visit http://sourceforge.net/projects/gameranger/"), wxT("About Gameranger"), wxOK);
	msgDlg->ShowModal();
	delete(msgDlg);
}
//--------------------------------------------------------------------------------
GRGameRoom *GRMainWindow::findGameRoom(wxUint32 id)
{
	wxUint32 x;
	for(x = 0; x < GameRooms.size(); x++)
	{
		if(GameRooms[x]->gameRoomID == id) return GameRooms[x];
	}
	return NULL;
}
//------------------------------------------------------------------------------
void GRMainWindow::updateGameRoomPlayerCountString(GRGameRoom *room)
{
	wxInt32 index;
	index = getGameItemIndex(room);
	if(index == -1) return;
	gameRoomList->SetItem(index, 4, wxString::Format(wxT("%d/%d"), room->currentPlayers, room->maxPlayers));
}
//-------------------------------------------------------------------------------
void GRMainWindow::playerLeftGameRoom(GR_PACKET *Packet)
{
	wxUint32 gameRoomID;
	wxUint8 *ptr;
	GRGameRoom *gameRoom;
	
	ptr = Packet->payload;

	memcpy(&gameRoomID, ptr, sizeof(wxUint32));
	gameRoomID = ntohl(gameRoomID);

	gameRoom = findGameRoom(gameRoomID);
	if(gameRoom == NULL) 
	{
		logWindow->logText(wxT("Unable to find game room, ID: ")+wxString::Format(wxT("%d\n"), gameRoomID));
		return;
	}

	gameRoom->currentPlayers--;
	updateGameRoomPlayerCountString(gameRoom);
}
//--------------------------------------------------------------------------------
wxInt32 GRMainWindow::getUserItemIndex(GRUser *user)
{
	wxInt32 x;
	GRUser *temp;
	for(x = 0; x < userListBox->GetItemCount(); x++)
	{
		temp = (GRUser*)userListBox->GetItemData(x);
		if(user == temp) return x;
	}
	GRLogger::getInstance()->log(GRLogger::LOG_WARNING, wxString::Format(wxT("---Error: Couldnt' find list item for User ID: %d"), user->userID));
	return -1;
}
//-----------------------------------------------------------------------------
wxInt32 GRMainWindow::getGameItemIndex(GRGameRoom *room)
{
	wxInt32 x;
	GRGameRoom *temp;
	for(x = 0; x < gameRoomList->GetItemCount(); x++)
	{
		temp = (GRGameRoom*)gameRoomList->GetItemData(x);
		if(room == temp) return x;
	}
	GRLogger::getInstance()->log(GRLogger::LOG_WARNING, wxString::Format(wxT("---Error: Couldnt' find list item for game with id: %d"), room->gameRoomID));
	return -1;
}
//-----------------------------------------------------------------------------
void GRMainWindow::addTextWithColor(wxString str, wxColour color)
{
	chatTextCtrl->SetDefaultStyle(wxTextAttr(color, *wxWHITE));
	/* if(showTimestamps) chatTextCtrl->AppendText("TIME: "); */
	chatTextCtrl->AppendText(str);
	chatTextCtrl->SetDefaultStyle(*wxRED);

	chatTextCtrl->SetInsertionPointEnd();

}
//-----------------------------------------------------------------------------
int wxCALLBACK wxListCompareFunction(long item1, long item2, long sortData)
{
	if(item1 == item2) return 0;
	else if(item1 < item2) return -1;
	else if(item1 > item2) return 1;
	return 0;
}
//-----------------------------------------------------------------------------
void GRMainWindow::OnUserDoubleClick(wxListEvent& event)
{
	if(event.GetIndex() == -1) return;
	GRUser *user;
	GRPrivateMessage *msg;
	user = (GRUser*)userListBox->GetItemData(event.GetIndex());
	if(user == NULL) return;

	//See if message window is already open, and if so, give it focus
	msg = findPrivateMessageByID(user->userID);
	if(msg != NULL) //it's open, give it focus
	{
		msg->SetFocus();
		return;
	}

	//Create new PM Window
	GRPrivateMessage *pm = new GRPrivateMessage(this, wxT("Private Message - ")+user->nick, wxDefaultPosition, wxDefaultSize);
	pm->mainWindow = this;
	pm->userID = user->userID;
	pm->nickname = user->nick;
	pm->Show(true);
	privateMessages.push_back(pm);
}
//------------------------------------------------------------------------------
GRPrivateMessage *GRMainWindow::findPrivateMessageByID(wxUint32 userID)
{
	wxUint32 x;
	GRPrivateMessage *msg;
	for(x = 0; x < privateMessages.size(); x++)
	{
		msg = privateMessages[x];
		if(msg->userID == userID) return msg;
	}
	return NULL;
}
//------------------------------------------------------------------------------
void GRMainWindow::receivedPrivateMessage(GR_PACKET *Packet)
{
	wxUint32 userID;
	wxString nickname;
	wxString message;
	wxUint8 *ptr;
	GRPrivateMessage *msg;

	ptr = Packet->payload;

	memcpy(&userID, ptr, sizeof(wxUint32));
	userID = wxUINT32_SWAP_ON_LE(userID);
	ptr += sizeof(wxUint32);

	nickname = bufToStr(ptr);
	ptr += nickname.Len() + 1;

	message = bufToStr(ptr);
	ptr += message.Len() + 1;

	//See if message window is already open, and if so, give it focus and add message
	msg = findPrivateMessageByID(userID);
	if(msg != NULL) //it's open, give it focus
	{
		msg->addTextWithColor(nickname, *wxBLUE);
		msg->addTextWithColor(wxT(": ")+message+wxT("\n"), *wxBLACK);
		return;
	}

	//Create new PM Window
	GRPrivateMessage *pm = new GRPrivateMessage(this, wxT("Private Message - ")+nickname, wxDefaultPosition, wxDefaultSize);
	pm->mainWindow = this;
	pm->userID = userID;
	pm->nickname = nickname;
	pm->Show(true);
	pm->addTextWithColor(nickname, *wxBLUE);
	pm->addTextWithColor(wxT(": ")+message+wxT("\n"), *wxBLACK);
	pm->chatEditField->SetFocus();
	privateMessages.push_back(pm);
}
/*-----------------------------------------------------------------------------*/
void GRMainWindow::OnGameRoomDoubleClick(wxListEvent& event)
{
	if(event.GetIndex() == -1) return;

	GRGameRoom *room;
	room = (GRGameRoom*)gameRoomList->GetItemData(event.GetIndex());
	if(room == NULL) return;

	wxUint32 roomID;
	wxUint8 null[1] = {0x00};
	wxUint32 len = 1;
	wxUint8 *payload;
	wxString password = wxT("");

	if(room->isLocked()) {
		wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Please enter the password for the game room"), wxT("Game room password required"), wxT(""),wxOK | wxCANCEL);
		if(dlg->ShowModal() == wxID_OK) {
			password = dlg->GetValue();
			len += password.Length();
			delete(dlg);
		}
		else {
			delete(dlg);
			return;
		}
	}

	len += sizeof(wxUint32);
	payload = new wxUint8[len];
	
	roomID = room->gameRoomID;
	roomID = wxUINT32_SWAP_ON_LE(roomID);

	memcpy(payload, (wxUint8*)&roomID, sizeof(wxUint32));

	if(room->isLocked()) {
		memcpy(payload+sizeof(wxUint32), (wxUint8*)(const char*)password.mb_str(), password.Length()+1);
	}
	else {
		memcpy(payload+sizeof(wxUint32), null, 1);
	}

	GRApplication::getInstance()->getMainConnection()->sendGRPacket(JOIN_GAME_ROOM, len, payload);
	delete[] payload;
}
//---------------------------------------------------------------------------
void GRMainWindow::gameRoomUserList(GR_PACKET *Packet)
{
	wxUint8 *ptr;
	wxUint32 userCount;
	GRGameRoom *room;
	GRUser *user;
	wxUint32 userID;
	wxInt32 x;
	wxUint32 iconID;
	wxString nickname;
	wxUint8 status;
	wxUint32 roomID;
	wxIcon roomIcon;
	GRGameRoomWindow *gameRoom = NULL;
	vector<GRUser*> roomUsers;

	ptr = Packet->payload;

	//game room id
	memcpy(&roomID, ptr, sizeof(wxUint32));
	roomID = wxUINT32_SWAP_ON_LE(roomID);
	ptr += sizeof(wxUint32);

	room = findGameRoom(roomID);
	if(room == NULL) return;

	

	//user count
	memcpy(&userCount, ptr, sizeof(wxUint32));
	userCount = wxUINT32_SWAP_ON_LE(userCount);
	ptr += sizeof(wxUint32);

	

	if(currentGameRoom == NULL) {
		gameRoom = new GRGameRoomWindow(this, room->Plugin->gameName+wxT(" - ")+room->host, wxDefaultPosition, wxDefaultSize);
		gameRoom->mainWindow = this;
		gameRoom->gameRoom = room;
	

		if(room->Plugin->image != NULL) 
		{
			roomIcon.CopyFromBitmap(wxBitmap(*room->Plugin->image));
		}

		gameRoom->SetIcon(roomIcon);
		gameRoom->userListBox->SetImageList(iconImgList, wxIMAGE_LIST_SMALL);
		gameRoom->addTextWithColor(wxT("Description: ")+room->description+wxT("\n\n"), *wxRED);
		gameRoomWindows.push_back(gameRoom);
		currentGameRoom = gameRoom;
		currentGameRoom->actionButton->SetLabel(wxT("Join"));

		/* if game is started, enable button */
		if(currentGameRoom->gameRoom->isPlaying()) {
				currentGameRoom->actionButton->Enable(true);
			}
			else {
				currentGameRoom->actionButton->Enable(false);
			}
	}
	else {
		if(currentGameRoom->gameRoom->gameRoomID == roomID) { /* host ended game */
			gameRoom = currentGameRoom;		
			currentGameRoom->addTextWithColor(wxT("<< The game has been aborted >>\n"), *wxRED);
		}
		/* if we are hosting, make the button say Start */
		if(currentGameRoom->gameRoom->grID == this->myUserID) {
			currentGameRoom->actionButton->SetLabel(wxT("Start"));
			currentGameRoom->actionButton->Enable(true);
		}
		else {
			currentGameRoom->actionButton->SetLabel(wxT("Join"));
			
			/* if game is started, enable button */
			if(currentGameRoom->gameRoom->isPlaying()) {
				currentGameRoom->actionButton->Enable(true);
			}
			else {
				currentGameRoom->actionButton->Enable(false);
			}
		}
		currentGameRoom->SetFocus();
		currentGameRoom->deleteUsers();
		currentGameRoom->gameRoom->currentPlayers = 0;	
		
	}

	room->currentPlayers = userCount;

	for(x = 0; x < userCount; x++)
	{
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
		if(currentGameRoom->gameRoom->grID == userID) {
			currentGameRoom->gameRoom->status = status;
		}

		//nickname
		nickname = bufToStr(ptr);
		ptr += nickname.Len() + 1;

		/* make user */
		user = new GRUser(nickname, userID, iconID);

		//game list
		parseGamesListForUser(user, ptr);
		ptr += *(ptr)+1;

		
		user->SetStatus(status);
		roomUsers.push_back(user);
		
	}

	for(x = (roomUsers.size()-1); x >= 0; x--) {
		currentGameRoom->AddUser(roomUsers[x], false);
	}
	
	roomUsers.clear();

	setGameRoomListInfo(currentGameRoom->gameRoom);
	updateGameRoomPlayerCountString(currentGameRoom->gameRoom);
	
	currentRoomID = currentGameRoom->gameRoom->gameRoomID;
	if(gameRoomWillClose) {
		currentGameRoom->Show(false);
		currentGameRoom->Close();
		gameRoomWillClose = false;
	}
	else {
		currentGameRoom->Show(true);
	}
}
//------------------------------------------------------------------------
void GRMainWindow::OnChangeNickMenu(wxCommandEvent &event)
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Change nickname"),
		wxT("New nickname: "));
	wxString nickname;

	if(dlg->ShowModal() != wxID_OK) //user hit cancel
	{
		delete(dlg);
		return;
	}

	nickname = dlg->GetValue();
	delete(dlg);

	if(nickname.Len() == 0)
	{
		wxMessageDialog *msgdlg = new wxMessageDialog(this, wxT("Your nickname was not changed, error: You entered a blank nickname"), wxT("Error changing nickname"), wxICON_ERROR);
		msgdlg->ShowModal();
		delete(msgdlg);
		return;
	}

	GRApplication::getInstance()->getMainConnection()->sendGRPacket(CHANGE_NICKNAME, nickname.Len()+1, (wxUint8*)(const char*)nickname.mb_str());
}
//--------------------------------------------------------------------------------
void GRMainWindow::OnChangeNameMenu(wxCommandEvent &event)
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Change real name"),
		wxT("New real name: "));
	wxString name;

	if(dlg->ShowModal() != wxID_OK) //user hit cancel
	{
		delete(dlg);
		return;
	}

	name = dlg->GetValue();
	delete(dlg);

	if(name.Len() == 0)
	{
		wxMessageDialog *msgdlg = new wxMessageDialog(this, wxT("Your name was not changed, error: You entered a blank name"), wxT("Error changing name"), wxICON_ERROR);
		msgdlg->ShowModal();
		delete(msgdlg);
		return;
	}

	GRApplication::getInstance()->getMainConnection()->sendGRPacket(CHANGE_REAL_NAME, name.Len()+1, (wxUint8*)(const char*)name.mb_str());
}
//--------------------------------------------------------------------------------
void GRMainWindow::MenuLogout(wxCommandEvent &event)
{
	GRApplication::getInstance()->closeMainConnection();
	OnDisconnect();
}
//---------------------------------------------------------------------------------
void GRMainWindow::OnChangePassword(wxCommandEvent &event)
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Change password"),
		wxT("New password: "), wxT(""), wxTE_PASSWORD);
	wxString password;
	wxUint8 *payload;
	int len = 0;

	if(dlg->ShowModal() != wxID_OK) //user hit cancel
	{
		delete(dlg);
		return;
	}

	password = dlg->GetValue();
	delete(dlg);

	if(password.Len() == 0)
	{
		wxMessageDialog *msgdlg = new wxMessageDialog(this, wxT("Your password was not changed, error: You entered a blank password"), wxT("Error changing nickname"), wxICON_ERROR);
		msgdlg->ShowModal();
		delete(msgdlg);
		return;
	}

	//make packet
	len = strlen(GRpassword) + password.Len() + 2;
	payload = new wxUint8[len];
	strcpy((char*)payload, GRpassword);
	strcpy((char*)payload+strlen(GRpassword)+1,(const char*)password.mb_str());

	GRApplication::getInstance()->getMainConnection()->sendGRPacket(CHANGE_ACCOUNT_PASSWORD, len, payload);
	delete[] payload;
}
//--------------------------------------------------------------------------------
void GRMainWindow::logPacket(wxUint8 *buf, wxUint32 len)
{
	wxUint32 command;

	memcpy(&command, buf, sizeof(wxUint32));
	command = ntohl(command);

	wxFile *file = new wxFile(wxString::Format(wxT("data/%08X.bin"), command), wxFile::write);
	file->Write(buf, len);
	file->Close();
	delete(file);
}
//-----------------------------------------------------------------------------
wxUint8 *GRMainWindow::createRawPacket(wxUint32 command, wxUint32 len, wxUint8 *payload)
{
	wxUint8 *buf;
	wxUint32 command1, length;

	buf = new wxUint8[len+8];

	//host order
	command1 = htonl(command);
	length = htonl(len);

	memcpy(buf, (void*)&command1, sizeof(wxUint32));
	memcpy(buf+sizeof(wxUint32), (void*)&length, sizeof(wxUint32));
	memcpy(buf+(sizeof(wxUint32)*2), payload, len);
	
	return buf;
}
//---------------------------------------------------------------------------------
void GRMainWindow::OnUserRightClick(wxListEvent &event)
{
	wxPoint pt;
	if(event.GetIndex() == -1) return;

	pt = event.GetPoint();

	//make games list menu
	clearGamesListMenu();
	makeGameListMenu(userListBox, gameListMenu, event.GetIndex());

	userListBox->PopupMenu(popupMenu, pt);
}
//----------------------------------------------------------------------------------
void GRMainWindow::OnUserListGetInfo(wxCommandEvent &event)
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
	GRApplication::getInstance()->getMainConnection()->sendGRPacket(GET_USER_INFO, sizeof(wxUint32), (wxUint8*)&userID);
}
//---------------------------------------------------------------------------------
void GRMainWindow::OnChangeIconMenu(wxCommandEvent &event)
{
	GRChangeIconWindow *changeIcon = new GRChangeIconWindow(this, wxT("Change icon"), wxDefaultPosition, wxDefaultSize);
	changeIcon->m_mainWindow = this;
	changeIcon->Show(true);
}
//-----------------------------------------------------------------------------------
void GRMainWindow::OnFindPlayerMenu(wxCommandEvent &event)
{
	GRFindPlayerWindow *findPlayer = GRFindPlayerWindow::getInstance(this);
	findPlayer->Show(true);
	findPlayer->SetFocus();
}
//-----------------------------------------------------------------------------------
wxUint8 *GRMainWindow::makePluginsList()
{
	wxUint32 len = 0, pluginCount = 0;
	wxUint8 *pluginList = NULL;
	wxUint16 gameCode;
	wxUint16 count;
	bool working = true;
	int *array, x, y, pos = 0;

	//count of plugins
	pluginCount = Plugins.size();

	//calculate lengths, each plugin uses 6 bytes, 2 for game code, 4 for options
	len = ((pluginCount-2) * 6)+2; //we have chat in there

	//allocate memory
	pluginList = new wxUint8[len];

	//make our array and fill it
	array = new int[pluginCount];
	for(x = 0; x < (int)Plugins.size(); x++)
	{
		array[x] = Plugins[x]->gameCode;
	}

	//sort it
	sort(array, array+pluginCount);

	//count
	count = pluginCount-2;
	count = wxUINT16_SWAP_ON_LE(count);
	memcpy(pluginList+pos, &count, 2);
	pos += 2;

	//let's make packet
	for(y = 0; y < pluginCount; y++)
	{
		for(x = 0; x < Plugins.size(); x++)
		{
			if(array[y] == 0) continue;
			if(array[y] == 0xffffffff) continue;
			if(Plugins[x]->gameCode == array[y])
			{
				gameCode = Plugins[x]->gameCode;
				gameCode = wxUINT16_SWAP_ON_LE(gameCode);
				memcpy(pluginList+pos, &gameCode, sizeof(wxUint16));
				pos += sizeof(wxUint16);
				memcpy(pluginList+pos, Plugins[x]->unknownOption, 4);
				pos += 4;
				break;
			}
		}
	}
	//logWindow->logData(pluginList, len);

	return pluginList;
}
//-----------------------------------------------------------------------------------
void GRMainWindow::appBanner(GR_PACKET *Packet)
{
	wxUint32 bannerCount, bannerID, bannerLength;
	int x;
	wxUint8 *ptr;

	ptr = Packet->payload;

	//banner count
	memcpy(&bannerCount, ptr, sizeof(wxUint32));
	bannerCount = wxUINT32_SWAP_ON_LE(bannerCount);
	ptr += sizeof(wxUint32);

	for(x = 0; x < bannerCount; x++)
	{
		//banner id
		memcpy(&bannerID, ptr, sizeof(wxUint32));
		bannerID = wxUINT32_SWAP_ON_LE(bannerID);
		ptr += sizeof(wxUint32);

		//banner length
		memcpy(&bannerLength, ptr, sizeof(wxUint32));
		bannerLength = wxUINT32_SWAP_ON_LE(bannerLength);
		ptr += sizeof(wxUint32);

		//unknown
		ptr += 4;

		saveBanner(ptr, bannerLength, bannerID);
		ptr += bannerLength;
	}
}
//-----------------------------------------------------------------------------------
void GRMainWindow::saveBanner(wxUint8 *data, wxUint32 length, wxUint32 bannerID)
{
	const char *swfFile = "FWS";
    const char *gifFile = "GIF";
    const char *jpgFile = "\xff\xd8";
	wxString extension;

	if(memcmp(swfFile, data, 3) == 0)
		extension = wxT("swf");
	else if(memcmp(gifFile, data, 3) == 0)
		extension = wxT("gif");
	else if(memcmp(jpgFile, data, 2) == 0)
		extension = wxT("jpg");

	wxFile file(wxString::Format(wxT("banner%08X"), bannerID)+wxT(".")+extension, wxFile::write);

	file.Write(data, length);
	file.Close();
}
//------------------------------------------------------------------------------------
void GRMainWindow::parseGamesListForUser(GRUser *user, wxUint8 *buf)
{
	wxUint8 *ptr;
	wxUint8 count;
	GRPlugin *plugin;
	int x, y, offset = 1;

	ptr = buf;

	//count
	count = *ptr;
	ptr++;

	for(x = 0; x < count; x++)
	{
		if((*ptr & 1) == 1) 
			user->addGameToList(pluginManager->findPluginByCode(offset));
		if((*ptr & 2) == 2)
			user->addGameToList(pluginManager->findPluginByCode(offset+1));
		if((*ptr & 4) == 4)
			user->addGameToList(pluginManager->findPluginByCode(offset+2));
		if((*ptr & 8) == 8)
			user->addGameToList(pluginManager->findPluginByCode(offset+3));
		if((*ptr & 16) == 16)
			user->addGameToList(pluginManager->findPluginByCode(offset+4));
		if((*ptr & 32) == 32)
			user->addGameToList(pluginManager->findPluginByCode(offset+5));
		if((*ptr & 64) == 64)
			user->addGameToList(pluginManager->findPluginByCode(offset+6));
		if((*ptr & 128) == 128)
			user->addGameToList(pluginManager->findPluginByCode(offset+7));

		offset += 8;
		ptr++;
	}
}
//--------------------------------------------------------------------------------------
wxUint8 *GRMainWindow::makeGameList(vector <GRPlugin*> list)
{
	int *plgList,x,y,z;
	wxUint8 *buf = NULL;
	wxUint8 byteCount;
	wxUint8 temp;
	int listSize;
	wxUint8 t1;
	int offset = 0;

	if(list.size() == 0)
	{
		buf = new wxUint8[1];
		byteCount = 0;
		memcpy(buf, &byteCount, 1);
		return buf;
	}

	plgList = new int[list.size()];

	//fill plugin list array
	for(x = 0; x < list.size(); x++)
	{
		plgList[x] = list[x]->gameCode;
	}

	//sort
	sort(plgList, plgList+list.size());

	byteCount = (plgList[list.size()-1]/8) + 1;
	buf = new wxUint8[byteCount+1];

	memcpy(buf, &byteCount, 1);
	listSize = list.size();

	for(x = 0; x < byteCount; x++)
	{
		temp = 0;
		for(y = 0; y < 8; y++)
		{
			for(z = 0; z < listSize; z++)
			{
				if(plgList[z] > (x*8) && plgList[z] <= ((x+1)*8))
				{
				t1 = plgList[z] - (x*8);
				if(t1 == 1)
					temp |= 1;
				if(t1 == 2)
					temp |= 2;
				if(t1 == 3)
					temp |= 4;
				if(t1 == 4)
					temp |= 8;
				if(t1 == 5)
					temp |= 16;
				if(t1 == 6)
					temp |= 32;
				if(t1 == 7)
					temp |= 64;
				if(t1 == 8)
					temp |= 128;
				}
			}
			
		}
		memcpy(buf+(x+1), &temp, 1);
	}
	return buf;
}
//--------------------------------------------------------------------------------------
void GRMainWindow::clearGamesListMenu()
{
	popupMenu->Destroy(GAMES_LIST_MENU);
	gameListMenu = new wxMenu();
	popupMenu->Append(GAMES_LIST_MENU, wxT("Games"), gameListMenu);
}
//------------------------------------------------------------------------------------
void GRMainWindow::makeGameListMenu(wxListCtrl *list, wxMenu *gameMenu, int index)
{
	GRUser *user;
	int x;
	wxMenuItem *gameItem;

	if(index == -1) return;

	user = (GRUser*)list->GetItemData(index);
	if(user == NULL) return;

	

	if(user->gamesList.size() == 0)
	{
		gameMenu->Append(100, wxT("No games"));
		return;
	}

	sort(user->gamesList.begin(), user->gamesList.end(), sortPluginByName());

	for(x = 0; x < user->gamesList.size(); x++)
	{
		if(user->gamesList[x]->gameCode == 0xffffffff) continue;
		gameItem = new wxMenuItem(gameListMenu, x);
		gameItem->SetBitmap(wxBitmap(*user->gamesList[x]->image));
		//gameItem->SetText(user->gamesList[x]->gameName);
		gameItem->SetItemLabel(user->gamesList[x]->gameName);
		gameMenu->Append(gameItem);
	}
}
//--------------------------------------------------------------------------------------
void GRMainWindow::OnChangeMyGamesMenu(wxCommandEvent &event)
{
	GRChangeMyGames *changeGames = new GRChangeMyGames(this, wxT("My Games"), wxDefaultPosition, wxDefaultSize);
	changeGames->populateListBox();
	changeGames->Show(true);
}
//----------------------------------------------------------------------------------
void GRMainWindow::setGameRoomListInfo(GRGameRoom *gameRoom)
{
	wxListItem info;
	wxInt32 index;
	wxColor color;
	wxFont font;

	index = getGameItemIndex(gameRoom);
	if(index == -1) 
	{
		return;
	}

	info.SetId(index);
	gameRoomList->GetItem(info);

	if(gameRoom->isPremium()) font.SetWeight(wxBOLD);
	if(gameRoom->isIdle()) info.SetTextColour(*wxLIGHT_GREY);

	info.m_mask = wxLIST_MASK_TEXT;
	info.m_text = gameRoom->host;
	info.m_itemId = index;
	info.m_col = 1;
	info.SetFont(font);
	
	gameRoomList->SetItem(info);

	/* if game room is playing or not */
	if(gameRoom->isPlaying()) {
		#ifdef WIN32 //windows won't show subitem images correctly, so just display text
		gameRoomList->SetItem(index, 5, wxT("Yes"), -1);
		#else
		gameRoomList->SetItem(index, 5, wxT(""), 2);
		#endif
	}
	else {
		gameRoomList->SetItem(index, 5, wxT(""), -1);
	}
	/* If room is locked */
	if(gameRoom->isLocked())
	{
		#ifdef WIN32 //windows won't show subitem images correctly, so just display text
		gameRoomList->SetItem(index, 6, wxT("Yes"), -1);
		#else
		gameRoomList->SetItem(index, 6, wxT(""), 1);
		#endif
	}

}
//---------------------------------------------------------------------------
void GRMainWindow::OnHostRoomMenu(wxCommandEvent &event)
{
	GRHostWindow *hostWindow = new GRHostWindow(this, wxT("Host Room"), wxDefaultPosition, wxDefaultSize);
	hostWindow->mainWindow = this;
	hostWindow->initListBoxes();
	hostWindow->Show(true);
}
//---------------------------------------------------------------------------
void GRMainWindow::OnToggleTimestamp(wxCommandEvent &event)
{
	if(timestampItem->IsChecked()) {
		showTimestamps = true;
	}
	else {
		showTimestamps = false;
	}
}
//----------------------------------------------------------------------------------------



