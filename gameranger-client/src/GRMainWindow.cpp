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
#include "GRMainWindow.h"
#include "GRLoginWindow.h"
#include "GRLogWindow.h"
#include "GRSecurity.h"
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
#include "memdebug.h"

#define LOG_PACKETS


GRMainWindow::GRMainWindow(const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) NULL, -1, title, pos, size)
{
	//window specific stuff
	
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create Menu Bar
	fileMenu = new wxMenu();
	fileMenu->Append(FILE_MENU_EXIT, wxT("E&xit"));
	fileMenu->Append(FILE_MENU_LOGOUT, wxT("&Logout"));
	fileMenu->Enable(FILE_MENU_LOGOUT, false);
	
	//Accounts Bar
	accountsMenu = new wxMenu();
	accountsMenu->Append(REGISTER_MENU_ITEM, wxT("&Register"));
	accountsMenu->Append(LOGIN_MENU_ITEM, wxT("&Login"));

	//User list box popup menu
	popupMenu = new wxMenu();
    popupMenu->Append(USER_LIST_MENU_GET_INFO, wxT("Get &user info"));

	//Options Menu
	optionsMenu = new wxMenu();
	optionsMenu->Append(CHANGE_NICK_MENU_ITEM, wxT("Change &nickname"));
	optionsMenu->Append(CHANGE_NAME_MENU_ITEM, wxT("Change &real name"));
	optionsMenu->Append(CHANGE_PASSWORD_MENU_ITEM, wxT("Change &password"));

	aboutMenu = new wxMenu();
	aboutMenu->Append(ABOUT_MENU_ABOUT, wxT("&About"));

	menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(accountsMenu, wxT("&Accounts"));
	menuBar->Append(optionsMenu, wxT("&Options"));
	menuBar->Append(aboutMenu, wxT("&Help"));

	SetMenuBar(menuBar);
	

	/* ---------------------------- Create Controls --------------------------- */
	wxString path = wxGetCwd();


	wxInitAllImageHandlers();

	//Image List
	imgList = new wxImageList(16, 16);

	//Icons for games list
	wxBitmap pic(path+wxT("/images/players.xpm"), wxBITMAP_TYPE_XPM);
	imgList->Add(pic);
	wxBitmap pic1(path+wxT("/images/go.xpm"), wxBITMAP_TYPE_XPM);
	wxBitmap pic2(path+wxT("/images/lock.xpm"), wxBITMAP_TYPE_XPM);
	imgList->Add(pic2);
	imgList->Add(pic1);
	//Icon Image List
	iconImgList = new wxImageList(16, 16);
	wxBitmap pic3(path+wxT("/images/defaulticon.xpm"), wxBITMAP_TYPE_XPM);
	iconImgList->Add(pic3);
	

	createControls();

	CentreOnScreen();

	//Status Window
	statusWindow = new GRConnectStatusWindow(this, wxT("Connecting to server..."), wxDefaultPosition, wxDefaultSize);

	loginWindow = NULL;
	regWindow = NULL;

	//Log Window
	logWindow = new GRLogWindow(this, wxT("Log Window."), wxDefaultPosition, wxDefaultSize);
	logWindow->mainWindow = this;
#ifdef SHOW_LOG_WINDOW
	logWindow->Show(true);
#endif

	//Create socket and register event handler
	m_socket = new wxSocketClient();
	m_socket->SetEventHandler(*this, SOCKET_ID1);
	m_socket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_CONNECTION_FLAG | wxSOCKET_LOST_FLAG);
	m_socket->Notify(true);

	lastIndex = -1;

	loadColorProfile();
	loadPlugins();

	//Init icon cache
	iconCache = new GRIconCache();
	iconCache->colorTable = colorTable;
	iconCache->imgList = iconImgList;
	iconCache->LoadCache(wxT("Icon_Cache.bin"));

	//Make our timer
	timer = new wxTimer(this, TIMER_ID);
	
	packetCounter = 0;

	currentProfile = 0;
	currentGameRoom = 0;
	currentPremiumInfoWindow = NULL;

}
//-------------------------------------------------------------------------------------
GRMainWindow::~GRMainWindow()
{
	wxUint32 x;

	/* timer */
	delete(timer);

	/* Destroy socket */
	delete(m_socket);
	
	/* delete popup menu */
	delete(popupMenu);

	
	cleanupGR();


	for(x = 0; x < Plugins.size(); x++)
	{
		delete(Plugins[x]);
	}
	Plugins.clear();

	if(iconCache != NULL) 
	{
		iconCache->SaveCache(wxT("Icon_Cache.bin"));
		delete(iconCache);
	}

	if(colorTable != NULL) delete[] colorTable;

}
//------------------------------------------------------------------------------------
void GRMainWindow::cleanupGR()
{
	int x;
	for(x = 0; x < Lobbies.size(); x++)
	{
		delete(Lobbies[x]);
	}
	Lobbies.clear();
	for(x = 0; x < GameRooms.size(); x++)
	{
		delete(GameRooms[x]);
	}
	GameRooms.clear();
	for(x = 0; x < privateMessages.size(); x++)
	{
		delete(privateMessages[x]);
	}
	privateMessages.clear();
	for(x = 0; x < gameRoomWindows.size(); x++)
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
	testPacket(wxT("data/00000040.bin"));
	testPacket(wxT("data/000000D1.bin"));
	testPacket(wxT("data/000000D3.bin"));*/
	
}
//-------------------------------------------------------------------------------------
void GRMainWindow::createControls()
{
	//Main Sizer and Panel
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);


	gameRoomList = new wxListView(this, GAMELIST_ID, wxDefaultPosition,
		wxSize(500, 200), wxLC_REPORT | wxLC_SORT_ASCENDING | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT);
	gameRoomList->AssignImageList(imgList, wxIMAGE_LIST_SMALL);
	gameRoomList->InsertColumn(0, wxT("Game Name"), wxLIST_FORMAT_LEFT, 185);
	gameRoomList->InsertColumn(1, wxT("Host"), wxLIST_FORMAT_LEFT, 155);
	gameRoomList->InsertColumn(2, wxT("Description"), wxLIST_FORMAT_LEFT, 250);
	gameRoomList->InsertColumn(3, wxT("IP Address"), wxLIST_FORMAT_LEFT, 100);
	gameRoomList->InsertColumn(4, wxT(""), wxLIST_FORMAT_LEFT, 45);
	gameRoomList->SetColumnImage(4, 0);
	gameRoomList->InsertColumn(5, wxT(""), wxLIST_FORMAT_LEFT, 30);
	gameRoomList->SetColumnImage(5, 2);
	gameRoomList->InsertColumn(6, wxT(""), wxLIST_FORMAT_LEFT, 30);
	gameRoomList->SetColumnImage(6, 1);

	
	//Add Notebook to sizer
	//topSizer->Add(notebook, 2, wxALIGN_TOP | wxALIGN_CENTER | wxALL | wxGROW | wxEXPAND, 5);
	topSizer->Add(gameRoomList, 2, wxALIGN_TOP | wxALIGN_CENTER | wxALL | wxGROW | wxEXPAND, 5);
	

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

	chatEditField = new wxTextCtrl(this, CHATEDIT_ID, wxT(""), wxDefaultPosition,
		wxSize(50, 20), wxTE_PROCESS_ENTER);

	//User list listbox
	userListBox = new wxListCtrl(this, USERLISTBOX_ID, wxDefaultPosition,
		wxSize(200, 300), wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING  | wxLC_NO_HEADER | wxLC_ALIGN_LEFT);
	userListBox->AssignImageList(iconImgList, wxIMAGE_LIST_SMALL);
	userListBox->InsertColumn(0, wxT("nick"), wxLIST_FORMAT_LEFT, 175);

	//Chat Sizer and middle sizer
	wxBoxSizer *chatSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *middleSizer = new wxBoxSizer(wxVERTICAL);

	//Chat window and chat edit field
	middleSizer->Add(chatTextCtrl, 1, wxALIGN_CENTER | wxRIGHT | wxBOTTOM | wxGROW, 5);
	middleSizer->Add(chatEditField, 0, wxALIGN_CENTER | wxRIGHT | wxGROW, 5);

	//user list/combo box
	rightSizer->Add(lobbyComboBox, 0, wxALIGN_RIGHT | wxALIGN_TOP | wxRIGHT | wxBOTTOM | wxGROW, 5);
	rightSizer->Add(userListBox, 1, wxALIGN_BOTTOM | wxALIGN_RIGHT | wxRIGHT | wxBOTTOM | wxGROW, 5);

	//add to main chat sizer
	chatSizer->Add(middleSizer, 3, wxALIGN_CENTER | wxALL | wxGROW, 0);
	chatSizer->Add(rightSizer, 1, wxALIGN_RIGHT  | wxALL | wxGROW | wxEXPAND, 0);
	
	topSizer->Add(chatSizer, 3, wxBOTTOM | wxLEFT | wxRIGHT | wxGROW, 5);


	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(topSizer);
	topSizer->Fit(this);
	topSizer->SetSizeHints(this);
}
//--------------------------------------------------------------------------------
void GRMainWindow::MenuExit()
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
	logWindow->logText(wxT("Disconnected from GameRanger server"));	
	addTextWithColor(wxT("<< Disconnected from GameRanger >>\n"), *wxRED);

	if(m_socket != NULL) 
	{
		m_socket->Close();
		delete(m_socket);
		m_socket = NULL;
	}
	loggedIn = false;
	userListBox->DeleteAllItems();
	gameRoomList->DeleteAllItems();
	lobbyComboBox->Clear();
	
	cleanupGR();

	fileMenu->Enable(FILE_MENU_LOGOUT, false);

	timer->Stop();
	statusWindow->Show(false);
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

	//disconnect socket
	if(m_socket->LastCount() <= 0) 
	{
		OnDisconnect();
		return;
	}

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
		case CLIENT_VERIFY:
			clientVerify(Packet);
		break;

		case SERVER_VERIFY_ACK:
			statusWindow->statusIndicator->SetValue(4);
		break;

		case READY_TO_PROCESS:
			statusWindow->statusIndicator->SetValue(5);
			loginToGR(Packet);
		break;

		case INVALID_LOGIN_PASSWORD:
			statusWindow->Show(false);
			invalidPassword();
		break;

		case SERVER_MESSAGE:
			serverMessage(Packet);
		break;

		case BAN_TIME_LEFT:
			banTimeLeft(Packet);
		break;

		case MY_USER_INFO:
			myUserInfo(Packet);
		break;

		case PUBLIC_LOBBY_LIST:
			publicLobbyList(Packet);
		break;	

		case LOBBY_USER_LIST:
			lobbyUserList(Packet);
		break;

		case LOBBY_USER_ACTION:
			currentLobby->userAction(Packet);
		break;

		case USER_LEFT_SERVER:
			currentLobby->userLeftServer(Packet);
		break;

		case LOBBY_CHAT_MESSAGE:
			currentLobby->chatMessage(Packet, 0);
		break;	

		case LOBBY_CHAT_MESSAGE_ME:
			currentLobby->chatMessage(Packet, 1);
		break;

		case GAME_ROOMS_LIST:
			gameRoomsList(Packet);
		break;

		case GAME_ROOM_OPENED:
			gameRoomOpened(Packet);
		break;

		case GAME_ROOM_CLOSED:
			gameRoomClosed(Packet);
		break;

		case INVALID_ACCOUNT:
			invalidAccount();
		break;

		case USER_CHANGED_NICKNAME:
			currentLobby->userChangedNick(Packet);
		break;

		case USER_IS_IDLE:
			currentLobby->userIsIdle(Packet);
		break;

		case USER_IS_ACTIVE:
			currentLobby->userIsActive(Packet);
		break;

		case PLAYER_JOINED_GAME_ROOM:
			playerJoinedGameRoom(Packet);
		break;

		case PLAYER_LEFT_GAME_ROOM:
			playerLeftGameRoom(Packet);
		break;

		case RECEIVED_ICON_DATA:
			receivedIconData(Packet);
		break;

		case RCV_PRIVATE_MESSAGE:
			receivedPrivateMessage(Packet);
		break;

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

		case GAME_ROOM_FULL:
			msgDlg = new wxMessageDialog(this, wxT("The game room you are trying to join is full, please try again later."), wxT("Join room errord"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
		break;

		case GAME_ROOM_NO_LATE_JOINERS:
			msgDlg = new wxMessageDialog(this, wxT("The game room you are trying to join does not allow late joiners."), wxT("Join room errord"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
		break;

		case PASSWORD_CHANGE_SUCCESSFUL:
			msgDlg = new wxMessageDialog(this, wxT("Your password was successfully changed."), wxT("Password change"), wxICON_INFORMATION);
			msgDlg->ShowModal();
			delete(msgDlg);
		break;

		case REG_USER_INFO:
			regularUserInfo(Packet);
		break;

		case PREMIUM_USER_INFO:
			premiumUserInfo(Packet);
		break;

		case RECV_PREMIUM_USER_IMAGE:
			recvPremiumUserImage(Packet);
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
void GRMainWindow::invalidAccount()
{
	wxMessageDialog *msgDlg = new wxMessageDialog(this, wxT("The account you specified is either invalid or hasn't been activated, please check your email and try again."), wxT("Login Error"), wxICON_ERROR);
	msgDlg->ShowModal();
	delete(msgDlg);
	
	statusWindow->Show(false);
	//logWindow->Show(false);
	//this->Show(false);
}
//-----------------------------------------------------------------------------
void GRMainWindow::gameRoomsList(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 grID, iconID, gameCount;
	wxUint8 status, locked;
	wxUint32 x;
	wxString ip, description, nick;
	GR_GAME_ROOM_PACKET *roomHeader;

	buf = Packet->payload;

	//game room count
	memcpy(&gameCount, buf, sizeof(wxUint32));
	gameCount = ntohl(gameCount);
	buf += sizeof(wxUint32);

	for(x = 0; x < gameCount; x++)
	{
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
		description = bufToStr(buf);
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
		nick = bufToStr(buf);
		buf += nick.Len() + 1;

		//game room list?
		buf += *(buf)+1;

		addGameRoom(roomHeader->gameRoomID, roomHeader->gameCode, roomHeader->maxPlayers,
			roomHeader->currentPlayers, ip, locked, description, grID, iconID,
			status, nick, false);
		
		//logWindow->logText(nick + wxT(" ") + description + wxT(" ") + ip);
	}
}
//-----------------------------------------------------------------------
void GRMainWindow::lobbyUserList(GR_PACKET *Packet)
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

	buf = Packet->payload;

	buf += sizeof(wxUint32); //some padding

	//lobby id for this user list
	memcpy(&lobbyID, buf, sizeof(wxUint32));
	lobbyID = ntohl(lobbyID);
	buf += sizeof(wxUint32);

	loggedIn = true;

	currentLobby = findLobby(lobbyID);

	currentLobby->ClearUsers();
	userListBox->DeleteAllItems();

	chatTextCtrl->AppendText(wxT("______")+currentLobby->lobbyName+wxT("______\n"));

	lobbyComboBox->SetSelection(currentLobby->comboIndex);

	//user count
	memcpy(&userCount, buf, sizeof(wxUint32));
	userCount = ntohl(userCount);
	buf += sizeof(wxUint32)*2;

	currentLobby->userCount = 0;

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
		nick = bufToStr(buf);
		buf += nick.Len() + 1;

		//game list?
		buf += *(buf)+1;

		user = new GRUser(nick, userID, iconID);
		user->SetStatus(status);
		icon = iconCache->findIcon(iconID);
		user->icon = icon;
		if(icon == NULL) 
		{
			if(iconID != 0) missingIconCount++;
			imageIndex = 0;
		}
		else imageIndex = icon->imageIndex;

		//add to list box
		index = userListBox->InsertItem(userListBox->GetItemCount(), nick, imageIndex);
		if(index != -1) userListBox->SetItemData(index, (long)user);
		setListInfo(user);
		

		currentLobby->AddUser(user, 0);

		//logWindow->logText(nick);
	}
	currentLobby->updateComboString();
	/* TODO */
	requestMissingIconsFromUserList(missingIconCount);
}
//-----------------------------------------------------------------------------
void GRMainWindow::publicLobbyList(GR_PACKET *Packet)
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
		lobbyName = bufToStr(buf);
		buf += lobbyName.Len() + 1;

		lobby = new GRLobby(lobbyName, lobbyID, type);
		lobby->SetUserCount(users);
		lobby->comboIndex = lobbyComboBox->GetCount();
		lobby->mainWindow = this;

		comboString = lobby->lobbyName + wxT("  -  ") + wxString::Format(wxT("%d"), lobby->userCount);
		lobbyComboBox->Append(comboString);

		Lobbies.push_back(lobby);		
	}
}
//-------------------------------------------------------------------------------
void GRMainWindow::myUserInfo(GR_PACKET *Packet)
{
	wxUint8 *payload;
	wxUint32 userID;
	wxString nick, name, email;
	payload = Packet->payload;

	payload += sizeof(wxUint32); //unknown

	//user id
	memcpy(&userID, payload, sizeof(wxUint32));
	userID = ntohl(userID);
	payload += sizeof(wxUint32);

	//email
	email = bufToStr(payload);
	payload += email.Len() + 1;

	//nickname
	nick = bufToStr(payload);
	payload += nick.Len() + 1;


	//real name
	name = bufToStr(payload);
	payload += name.Len() + 1;

	myUserID = userID;
	myNickname = nick;
	
	if(loginWindow != NULL) 
	{
		loginWindow->currentProfile->email = email;
		loginWindow->currentProfile->nickname = nick;
		loginWindow->currentProfile->realname = name;
		loginWindow->currentProfile->grID = userID;
		loginWindow->currentProfile->Write();
	}
	currentProfile = loginWindow->currentProfile;
	statusWindow->Show(false);

}	
//---------------------------------------------------------------------------
void GRMainWindow::serverMessage(GR_PACKET *Packet)
{
	wxString message = bufToStr(Packet->payload);
	wxUint32 x;
	
	//replace the 0x0d's with 0x0a's(linux doesen't accept the carriage return well	
	for(x = 0; x < message.Len(); x++) 
	{
		if(message[x] == 0x0d) message[x] = 0x0a;
	}

	chatTextCtrl->AppendText(message);

	if(loginWindow != NULL) loginWindow->Close();
	statusWindow->Show(false);
}
//----------------------------------------------------------------------------
void GRMainWindow::banTimeLeft(GR_PACKET *Packet)
{
	GR_BAN_TIME_PACKET *banPacket;
	wxUint32 seconds_left, remain;
	wxUint32 days, hours, minutes, seconds;
	char tleft[64];

	banPacket = (GR_BAN_TIME_PACKET*)Packet->payload;

	banPacket->time_left = ntohl(banPacket->time_left);

	

	seconds_left = banPacket->time_left;

	days = seconds_left/86400;
	remain = seconds_left % 86400;
	hours = remain/3600;
	remain = seconds_left % 3600;
	minutes = remain/60;
	remain = seconds_left % 60;
	seconds = remain;

	sprintf(tleft, "%d days, %d hours, %d minutes, %d seconds", days, hours, minutes, seconds);
	wxString detail = bufToStr((wxUint8*)tleft);
	logWindow->logText(wxT("Ban detail: ") + detail);
}
//----------------------------------------------------------------------------
void GRMainWindow::invalidPassword()
{
	wxMessageDialog *msgDlg = new wxMessageDialog(this, wxT("The password you entered is invaild, please revise your password and try again."), wxT("Login Error"), wxICON_ERROR);
	msgDlg->ShowModal();
	delete(msgDlg);
}
//-------------------------------------------------------------------------------
void GRMainWindow::clientVerify(GR_PACKET *Packet)
{
	GR_CLIENT_AUTH_PACKET *payload;
	GR_CLIENT_AUTH_ACK_PACKET authAckPacket;

	payload = (GR_CLIENT_AUTH_PACKET*)Packet->payload;

	//convert endianess
	payload->code = ntohl(payload->code);
	payload->version = ntohl(payload->version);
	payload->version1 = ntohl(payload->version1);
	payload->version2 = ntohl(payload->version2);
	payload->version3 = ntohl(payload->version3);

	//emulate latest version log
	logWindow->logText(wxString::Format(wxT("Emulating client version %08X..."), payload->version));
	
	//Craft Response
	authAckPacket.clientVersion = htonl(payload->version);
	authAckPacket.verifyCode = htonl(encryptVerifyCode(payload->code));
	authAckPacket.padding = htonl(0);
	authAckPacket.srvVerifyCode = htonl(rand());

	m_verifyCode = authAckPacket.verifyCode;

	//logWindow->logData((wxUint8*)&authAckPacket, sizeof(authAckPacket));

	sendGRPacket(CLIENT_VERIFY_ACK, sizeof(authAckPacket), (wxUint8*)&authAckPacket);

	statusWindow->SetTitle(wxT("Checking versions..."));
	statusWindow->statusIndicator->SetValue(3);
}
//------------------------------------------------------------------------------
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
void GRMainWindow::loginToGR(GR_PACKET *Packet)
{
	GR_LOGIN_PACKET loginPacketHeader;
	wxUint32 ip = 0x7f000001;
	wxUint32 GRID;
	wxUint32 len = 0, pos = 0;
	wxUint8 *payload = NULL;
	bool emailLogin;

	if(myUserID == 0) emailLogin = true;
	else emailLogin = false;

	char temp[1];
	char *options = "\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x02\x00\x20\x01";
	char *location = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF"; //location
	
	char *x6cplugins = 
"\x90\x09\x21\x00\x00\x00\x00\x00\x81\x00\x01\x00"
"\x00\x00\x02\x00\x02\x00\x00\x00\x03\x00\x03" 
"\x00\x00\x00\x01\x00\x04\x00\x00\x00\x01\x00" 
"\x05\x00\x00\x00\x02\x00\x06\x00\x00\x00\x01" 
"\x00\x07\x00\x00\x00\x01\x00\x08\x00\x00\x00" 
"\x01\x00\x09\x00\x00\x00\x01\x00\x0A\x00\x00" 
"\x00\x01\x00\x0B\x00\x00\x00\x02\x00\x0C\x00" 
"\x00\x00\x01\x00\x0D\x00\x00\x00\x03\x00\x0E" 
"\x00\x00\x00\x01\x00\x0F\x00\x00\x00\x02\x00" 
"\x10\x00\x00\x00\x01\x00\x11\x00\x00\x00\x01" 
"\x00\x12\x00\x00\x00\x01\x00\x13\x00\x00\x00" 
"\x01\x00\x14\x00\x00\x00\x01\x00\x15\x00\x00" 
"\x00\x01\x00\x16\x00\x00\x00\x02\x00\x17\x00" 
"\x00\x00\x01\x00\x18\x00\x00\x00\x01\x00\x19" 
"\x00\x00\x00\x01\x00\x1A\x00\x00\x00\x01\x00" 
"\x1B\x00\x00\x00\x01\x00\x1C\x00\x00\x00\x02" 
"\x00\x1D\x00\x00\x00\x01\x00\x1E\x00\x00\x00" 
"\x01\x00\x1F\x00\x00\x00\x01\x00\x20\x00\x00" 
"\x00\x01\x00\x21\x00\x00\x00\x01\x00\x22\x00" 
"\x00\x00\x01\x00\x23\x00\x00\x00\x01\x00\x24" 
"\x00\x00\x00\x01\x00\x25\x00\x00\x00\x02\x00" 
"\x26\x00\x00\x00\x01\x00\x27\x00\x00\x00\x01" 
"\x00\x28\x00\x00\x00\x01\x00\x29\x00\x00\x00" 
"\x01\x00\x2A\x00\x00\x00\x01\x00\x2B\x00\x00" 
"\x00\x01\x00\x2C\x00\x00\x00\x01\x00\x2D\x00" 
"\x00\x00\x01\x00\x2E\x00\x00\x00\x01\x00\x2F" 
"\x00\x00\x00\x01\x00\x30\x00\x00\x00\x01\x00" 
"\x31\x00\x00\x00\x01\x00\x32\x00\x00\x00\x01" 
"\x00\x34\x00\x00\x00\x01\x00\x35\x00\x00\x00" 
"\x01\x00\x36\x00\x00\x00\x01\x00\x37\x00\x00" 
"\x00\x01\x00\x38\x00\x00\x00\x01\x00\x39\x00" 
"\x00\x00\x01\x00\x3A\x00\x00\x00\x03\x00\x3B" 
"\x00\x00\x00\x03\x00\x3C\x00\x00\x00\x03\x00" 
"\x3D\x00\x00\x00\x01\x00\x3E\x00\x00\x00\x02" 
"\x00\x3F\x00\x00\x00\x01\x00\x40\x00\x00\x00" 
"\x01\x00\x41\x00\x00\x00\x01\x00\x42\x00\x00" 
"\x00\x01\x00\x43\x00\x00\x00\x01\x00\x44\x00" 
"\x00\x00\x01\x00\x45\x00\x00\x00\x01\x00\x46" 
"\x00\x00\x00\x02\x00\x47\x00\x00\x00\x01\x00" 
"\x48\x00\x00\x00\x01\x00\x49\x00\x00\x00\x01" 
"\x00\x4A\x00\x00\x00\x01\x00\x4B\x00\x00\x00" 
"\x01\x00\x4D\x00\x00\x00\x01\x00\x4E\x00\x00" 
"\x00\x01\x00\x4F\x00\x00\x00\x01\x00\x50\x00" 
"\x00\x00\x01\x00\x51\x00\x00\x00\x01\x00\x52" 
"\x00\x00\x00\x01\x00\x53\x00\x00\x00\x01\x00" 
"\x54\x00\x00\x00\x02\x00\x55\x00\x00\x00\x01" 
"\x00\x56\x00\x00\x00\x01\x00\x57\x00\x00\x00" 
"\x01\x00\x58\x00\x00\x00\x02\x00\x59\x00\x00" 
"\x00\x01\x00\x5A\x00\x00\x00\x01\x00\x5B\x00" 
"\x00\x00\x01\x00\x5C\x00\x00\x00\x01\x00\x5E"
"\x00\x00\x00\x01\x00\x60\x00\x00\x00\x01\x00" 
"\x61\x00\x00\x00\x02\x00\x62\x00\x00\x00\x01" 
"\x00\x63\x00\x00\x00\x01\x00\x64\x00\x00\x00" 
"\x01\x00\x65\x00\x00\x00\x01\x00\x66\x00\x00" 
"\x00\x04\x00\x67\x00\x00\x00\x02\x00\x68\x00" 
"\x00\x00\x01\x00\x69\x00\x00\x00\x03\x00\x6A" 
"\x00\x00\x00\x01\x00\x6B\x00\x00\x00\x01\x00" 
"\x6C\x00\x00\x00\x02\x00\x6E\x00\x00\x00\x06" 
"\x00\x6F\x00\x00\x00\x07\x00\x71\x00\x00\x00" 
"\x01\x00\x72\x00\x00\x00\x04\x00\x73\x00\x00" 
"\x00\x02\x00\x74\x00\x00\x00\x03\x00\x75\x00" 
"\x00\x00\x02\x00\x76\x00\x00\x00\x01\x00\x77" 
"\x00\x00\x00\x01\x00\x78\x00\x00\x00\x01\x00" 
"\x79\x00\x00\x00\x02\x00\x7B\x00\x00\x00\x01" 
"\x00\x7C\x00\x00\x00\x02\x00\x7D\x00\x00\x00" 
"\x02\x00\x7E\x00\x00\x00\x01\x00\x7F\x00\x00" 
"\x00\x01\x00\x80\x00\x00\x00\x02\x00\x81\x00" 
"\x00\x00\x03\x00\x82\x00\x00\x00\x01\x00\x83" 
"\x00\x00\x00\x01\x00\x84\x00\x00\x00\x01\x00" 
"\x85\x00\x00\x00\x01\x00\x86\x00\x00\x00\x01" 
"\x00\x88\x00\x00\x00\x01\x00\x89\x00\x00\x00" 
"\x01\x3F\x20\x91\x3C";

	statusWindow->SetTitle(wxT("Logging in..."));

	if(emailLogin == true) loginPacketHeader.loginType = 2; //email based login
	else loginPacketHeader.loginType = 1;

	GRID = htonl(myUserID);
	if(emailLogin == true) loginPacketHeader.previousGRID = htonl(0);
	else loginPacketHeader.previousGRID = GRID;

	//mac
	loginPacketHeader.macAddress[0] = loginWindow->currentProfile->macAddress[0];
	loginPacketHeader.macAddress[1] = loginWindow->currentProfile->macAddress[1];
	loginPacketHeader.macAddress[2] = loginWindow->currentProfile->macAddress[2];
	loginPacketHeader.macAddress[3] = loginWindow->currentProfile->macAddress[3];
	loginPacketHeader.macAddress[4] = loginWindow->currentProfile->macAddress[4];
	loginPacketHeader.macAddress[5] = loginWindow->currentProfile->macAddress[5];

	//xor each byte
	for(wxUint32 x = 0; x < 6; x++) loginPacketHeader.macAddress[x] ^= 0x77;

	if(emailLogin == true) loginPacketHeader.checksum = htonl(Algorithm((char*)&m_verifyCode, (char*)&loginPacketHeader.macAddress, 0, ip, 2));
	else loginPacketHeader.checksum = htonl(Algorithm((char*)&m_verifyCode, (char*)&loginPacketHeader.macAddress, ntohl(GRID), ip, 2));
	ip = htonl(ip);
	ip ^= 0xffffffff;

	//construct packet
	len += sizeof(wxUint8); //login type
	len += sizeof(wxUint32); //gr id
	len += 6; //mac address size
	len += 4; //lan ip
	len += sizeof(wxUint32); //checksum

	len += strlen(GRpassword) + 1;
	
	if(emailLogin == true) len += strlen(GRemail) + 1;
	else len += sizeof(wxUint32); //gr id

	len += 22; //options
	len += 12; //location
	len += 787; //x6cplugins

	payload = new wxUint8[len];

	/* construct packet */

	//login type
	memcpy(payload+pos, &loginPacketHeader.loginType, 1);
	pos += 1;

	//previous gr id
	memcpy(payload+pos, &loginPacketHeader.previousGRID, sizeof(wxUint32));
	pos += sizeof(wxUint32);

	//mac address
	memcpy(payload+pos, &loginPacketHeader.macAddress, 6); 
	pos += 6;

	//lan ip
	memcpy(payload+pos, &ip, 4);
	pos += 4;

	//checksum
	memcpy(payload+pos, &loginPacketHeader.checksum, sizeof(wxUint32));
	pos += sizeof(wxUint32);
	
	//password 
	temp[0] = (wxUint8)strlen(GRpassword);
	memcpy(payload+pos, temp, 1);
	pos++;
	memcpy(payload+pos, GRpassword, strlen(GRpassword));
	pos += strlen(GRpassword);

	if(emailLogin == true) {
		//email
		temp[0] = (wxUint8)strlen(GRemail);
		memcpy(payload+pos, temp, 1);
		pos++;
		memcpy(payload+pos, GRemail, strlen(GRemail));
		pos += strlen(GRemail);
	}
	else {
		//gr id
		memcpy(payload+pos, &GRID, sizeof(wxUint32));
		pos += sizeof(wxUint32);
	}

	//options
	memcpy(payload+pos, options, 22);
	pos += 22;

	//location
	memcpy(payload+pos, location, 12);
	pos += 12;

	//plugins
	memcpy(payload+pos, x6cplugins, 787);
	pos += 787;

	//from last packet, copy to end
	memcpy(payload+pos-4, Packet->payload, 4);

	//logWindow->logData(payload, pos);

	sendGRPacket(LOGIN_TO_GAMERANGER, pos, payload);

	delete[] payload;
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------
void GRMainWindow::testPacket(wxString filename)
{
	wxFile *file = new wxFile();
	GR_PACKET_HEADER header;
	GR_PACKET packet;
	wxUint8 *buf;
	wxUint32 len;

	if(!file->Exists(filename)) 
	{
		delete(file);
		return;
	}

	file->Open(filename, wxFile::read);

	len = file->Length();
	buf = new wxUint8[len];

	file->Read(buf, (wxUint32)len);

	memcpy(&header, buf, sizeof(GR_PACKET_HEADER));
	header.command = ntohl(header.command);
	header.payloadLength = ntohl(header.payloadLength);

	packet.header = &header;
	packet.payload = (wxUint8*)buf+sizeof(GR_PACKET_HEADER);

	handlePacket(&packet);

	file->Close();
	delete[] buf;
	delete(file);
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
void GRMainWindow::loadPlugins()
{
	wxDir dir;
	wxString filename;
	GRPlugin *plugin;

	if(!dir.Exists(wxGetCwd()+wxT("/plugins"))) return;

	dir.Open(wxGetCwd()+wxT("/plugins"));

	if(!dir.IsOpened()) 
	{
		logWindow->logText(wxT("Unable to open plugins directory."));
		return;
	}

	bool cont = dir.GetFirst(&filename, wxT(""), wxDIR_FILES);
	while(cont)
	{
		plugin = new GRPlugin();
		plugin->colorTable = colorTable;
		plugin->LoadFromFile(wxGetCwd()+wxT("/plugins/")+filename);
		//logWindow->logText(plugin->gameName);
		plugin->imageIndex = imgList->Add(*plugin->image);
		Plugins.push_back(plugin);
		cont = dir.GetNext(&filename);
	}
	//add chat plugin
	plugin = new GRPlugin();
	plugin->gameCode = 0;
	plugin->gameName = wxT("Chat");
	Plugins.push_back(plugin);
	
	//add default if we don't have it
	plugin = new GRPlugin();
	plugin->gameCode = 0xffffffff;
	plugin->gameName = wxT("Unknown Game");
	Plugins.push_back(plugin);
}
//-------------------------------------------------------------------------
void GRMainWindow::addGameRoom(wxUint32 gameRoomID, wxUint32 gameCode, wxUint32 maxPlayers, wxUint32 currentPlayers, wxString ip, wxUint8 locked, wxString description, wxUint32 grID, wxUint32 iconID, wxUint32 status, wxString host, bool announce)
{
	int index;
	GRGameRoom *room;
	GRPlugin *plugin;
	wxString str;
	plugin = findPluginByCode(gameCode);
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
	gameRoomList->SetItemData(index, (long)room);
	GameRooms.push_back(room);

	if(announce == true) 
	{	
		str = wxT("<< ") + room->host + wxT(" has started a ") + room->Plugin->gameName + wxT(" room >>");
		chatTextCtrl->AppendText(str+wxT("\n"));
	}
}
//-----------------------------------------------------------------------------
GRPlugin *GRMainWindow::findPluginByCode(wxUint32 gameCode)
{
	wxUint32 x;
	GRPlugin *defaultPlugin = NULL;
	for(x = 0; x < Plugins.size(); x++)
	{
		if(Plugins[x]->gameCode == gameCode) return Plugins[x];
		if(Plugins[x]->gameCode == 0xffffffff) defaultPlugin = Plugins[x];
	}
	return defaultPlugin;
}
//----------------------------------------------------------------------------
void GRMainWindow::loadColorProfile()
{
	wxFile file;
	wxUint32 size;

	colorTable = NULL;
	if(!file.Exists(wxT("macos.act"))) return;

	file.Open(wxT("macos.act"), wxFile::read);
	if(!file.IsOpened()) return;

	size = file.Length();

	colorTable = new wxUint8[size];
	if(colorTable == NULL) return;

	file.Read(colorTable, size);
	file.Close();
	logWindow->logText(wxT("Loaded color profile."));
}
//---------------------------------------------------------------------------
void GRMainWindow::gameRoomOpened(GR_PACKET *Packet)
{
	wxUint8 *buf;
	wxUint32 grID, iconID;
	wxUint8 status, locked;
	wxString ip, description, nick;
	GR_GAME_ROOM_PACKET *roomHeader;

	buf = Packet->payload;


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
		description = bufToStr(buf);
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
		nick = bufToStr(buf);
		buf += nick.Len() + 1;

		//game room list?
		buf += *(buf)+1;

		addGameRoom(roomHeader->gameRoomID, roomHeader->gameCode, roomHeader->maxPlayers,
			roomHeader->currentPlayers, ip, locked, description, grID, iconID,
			status, nick, true);
		
}
//---------------------------------------------------------------------------
void GRMainWindow::gameRoomClosed(GR_PACKET *Packet)
{
	GRGameRoom *room = NULL;
	vector<GRGameRoom*>::iterator it;
	wxUint32 roomID;
	wxUint32 x;
	wxInt32 index;
	wxString str;

	memcpy(&roomID, Packet->payload, sizeof(wxUint32));
	roomID = ntohl(roomID);

	for(x = 0; x < GameRooms.size(); x++)
	{
		room = GameRooms[x];
		if(room->gameRoomID == roomID) 
		{
			index = getGameItemIndex(room);
			if(index == -1)
			{
				delete(room);
				return;
			}
			gameRoomList->DeleteItem(index);
			str = wxT("<< ") + room->host + wxT(" has closed a ") + room->Plugin->gameName + wxT(" room >>");
			chatTextCtrl->AppendText(str+wxT("\n"));
			it = GameRooms.begin()+x;
			GameRooms.erase(it);
			delete(room);
			return;
		}	
	}
}
//----------------------------------------------------------------------------
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
	wxUint32  x;
	wxInt32 index;

	index = lobbyComboBox->GetSelection();
	
	if(index == -1) return;

	if(index == lastIndex) return;

	for(x = 0; x < Lobbies.size(); x++)
	{
		if(Lobbies[x]->comboIndex == index) 
		{
			joinLobby(Lobbies[x]);
			lastIndex = index;
			return;
		}
	}

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
void GRMainWindow::OnTimer()
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
	if(chatEditField->GetValue().Len() == 0) return;
	if(currentProfile == NULL) return;
	sendGRPacket(SEND_LOBBY_CHAT_MESSAGE, chatEditField->GetValue().Len()+1, (wxUint8*)(const char*)chatEditField->GetValue().mb_str());

	addTextWithColor(myNickname, *wxRED);
	addTextWithColor(wxT(": ") + chatEditField->GetValue() + wxT("\n"), *wxBLACK);

	chatEditField->SetValue(wxT(""));
}
//-----------------------------------------------------------------------------
void GRMainWindow::OnLoginMenu(wxCommandEvent &event)
{
	//there is already a login window open
	if(loginWindow != NULL) 
	{
		loginWindow->SetFocus();
		return;
	}
		
	//Login Window
	loginWindow = new GRLoginWindow(this, wxT("Login to GameRanger"), wxDefaultPosition,
			wxSize(200, 200));
	loginWindow->mainWindow = this;
	loginWindow->Show(true);

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
void GRMainWindow::playerJoinedGameRoom(GR_PACKET *Packet)
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

	gameRoom->currentPlayers++;
	updateGameRoomPlayerCountString(gameRoom);
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
void GRMainWindow::requestMissingIconsFromUserList(wxUint32 count)
{
	wxUint32 missingCount = 0, len = 0, pos = 0, x;
	wxUint32 iconID;
	wxUint8 *buf;

	//If we don't need any icons, no point in sending it
	if(count == 0) return;

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

	sendGRPacket(REQUEST_ICON_DATA, len, buf);
	delete[] buf;
}
//------------------------------------------------------------------------------
void GRMainWindow::receivedIconData(GR_PACKET *Packet)
{
	wxUint32 count, x, iconID;
	wxUint8 *ptr;
	GRIcon *icon;

	ptr = Packet->payload;
	
	memcpy(&count, ptr, sizeof(wxUint32));
	count = ntohl(count);
	ptr += sizeof(wxUint32);

	for(x = 0; x < count; x++)
	{
		memcpy(&iconID, ptr, sizeof(wxUint32));
		iconID = ntohl(iconID);
		ptr += sizeof(wxUint32);
	
		icon = new GRIcon();
		icon->colorTable = colorTable;
		icon->SetIconID(iconID);
		icon->SetIconData(ptr);
		icon->imageIndex = iconImgList->Add(*icon->image);
		iconCache->AddIcon(icon);

		ptr += 288;
	}
	purgeIcons();
}
//-----------------------------------------------------------------------------
void GRMainWindow::purgeIcons()
{
	wxUint32 x;
	wxInt32 index;
	GRUser *user;

	for(x = 0; x < currentLobby->Users.size(); x++)
	{
		user = currentLobby->Users[x];
		if(user->icon == NULL && user->iconID != 0) 
		{
			user->icon = iconCache->findIcon(user->iconID);
			if(user->icon != NULL) 
			{
				index = getUserItemIndex(user);
				if(index != -1) 
				{
					userListBox->SetItemImage(index, user->icon->imageIndex, user->icon->imageIndex);
				}
			}
		}
	}


}
//------------------------------------------------------------------------------
wxInt32 GRMainWindow::getUserItemIndex(GRUser *user)
{
	wxInt32 x;
	GRUser *temp;
	for(x = 0; x < userListBox->GetItemCount(); x++)
	{
		temp = (GRUser*)userListBox->GetItemData(x);
		if(user == temp) return x;
	}
	logWindow->logText(wxString::Format(wxT("---Error: Couldnt' find list item for User ID: %d"), user->userID));
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
	logWindow->logText(wxString::Format(wxT("---Error: Couldnt' find list item for game with id: %d"), room->gameRoomID));
	return -1;
}
//-----------------------------------------------------------------------------
void GRMainWindow::addTextWithColor(wxString str, wxColour color)
{
	chatTextCtrl->SetDefaultStyle(wxTextAttr(color, *wxWHITE));
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
void GRMainWindow::requestIcon(wxUint32 iconID)
{
	wxUint32 len = 0, pos = 0, count;
	wxUint8 *buf;

	count = 1;
	count = htonl(count);


	len = sizeof(wxUint32)*2;

	buf = new wxUint8[len];

	memcpy(buf, (wxUint8*)&count, sizeof(wxUint32));
	pos += sizeof(wxUint32);

	iconID = htonl(iconID);
	memcpy(buf+pos, (wxUint8*)&iconID, sizeof(wxUint32));
	pos += sizeof(wxUint32);

	sendGRPacket(REQUEST_ICON_DATA, len, buf);
	delete[] buf;
}
//------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
void GRMainWindow::OnGameRoomDoubleClick(wxListEvent& event)
{
	if(event.GetIndex() == -1) return;

	GRGameRoom *room;
	room = (GRGameRoom*)gameRoomList->GetItemData(event.GetIndex());
	if(room == NULL) return;

	wxUint32 roomID;
	wxUint8 null[1] = {0x00};
	wxUint8 *payload = new wxUint8[sizeof(wxUint32)+1];
	
	roomID = room->gameRoomID;
	roomID = wxUINT32_SWAP_ON_LE(roomID);

	memcpy(payload, (wxUint8*)&roomID, sizeof(wxUint32));
	memcpy(payload+sizeof(wxUint32), null, 1);

	sendGRPacket(JOIN_GAME_ROOM, sizeof(wxUint32)+1, payload);
	delete[] payload;
}
//---------------------------------------------------------------------------
void GRMainWindow::gameRoomUserList(GR_PACKET *Packet)
{
	wxUint8 *ptr;
	wxUint32 userCount;
	GRGameRoom *room;
	GRUser *user;
	wxUint32 userID, x;
	wxUint32 iconID;
	wxString nickname;
	wxUint8 status;
	wxUint32 roomID;
	wxIcon roomIcon;

	ptr = Packet->payload;

	//game room id
	memcpy(&roomID, ptr, sizeof(wxUint32));
	roomID = wxUINT32_SWAP_ON_LE(roomID);
	ptr += sizeof(wxUint32);

	room = findGameRoom(roomID);
	if(room == NULL) return;

	room->currentPlayers++;
	updateGameRoomPlayerCountString(room);

	//user count
	memcpy(&userCount, ptr, sizeof(wxUint32));
	userCount = wxUINT32_SWAP_ON_LE(userCount);
	ptr += sizeof(wxUint32);

	GRGameRoomWindow *gameRoom = new GRGameRoomWindow(this, room->Plugin->gameName+wxT(" - ")+room->host, wxDefaultPosition, wxDefaultSize);
	gameRoom->mainWindow = this;
	gameRoom->gameRoom = room;
	

	if(room->Plugin->image != NULL) 
	{
		roomIcon.CopyFromBitmap(wxBitmap(room->Plugin->image));
	}

	gameRoom->SetIcon(roomIcon);
	gameRoom->userListBox->SetImageList(iconImgList, wxIMAGE_LIST_SMALL);
	gameRoom->addTextWithColor(wxT("Description: ")+room->description+wxT("\n\n"), *wxRED);
	gameRoomWindows.push_back(gameRoom);
	currentGameRoom = gameRoom;

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

		//nickname
		nickname = bufToStr(ptr);
		ptr += nickname.Len() + 1;

		//game list
		ptr += *(ptr)+1;

		user = new GRUser(nickname, userID, iconID);
		user->SetStatus(status);
		gameRoom->AddUser(user, false);
	}

	gameRoom->Show(true);
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

	sendGRPacket(CHANGE_NICKNAME, nickname.Len()+1, (wxUint8*)(const char*)nickname.mb_str());
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

	sendGRPacket(CHANGE_REAL_NAME, name.Len()+1, (wxUint8*)(const char*)name.mb_str());
}
//--------------------------------------------------------------------------------
void GRMainWindow::MenuLogout()
{
	m_socket->Close();
	delete(m_socket);
	m_socket = NULL;
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

	sendGRPacket(CHANGE_ACCOUNT_PASSWORD, len, payload);
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
	sendGRPacket(GET_USER_INFO, sizeof(wxUint32), (wxUint8*)&userID);
}
//----------------------------------------------------------------------------------
void GRMainWindow::regularUserInfo(GR_PACKET *Packet)
{
	wxUint32 roomID;
	wxString nickname, realName, email;
	wxUint8 *ptr;
	GR_USER_INFO_HEADER infoHeader;
	GRUserInfo *userInfo;
	GRUserInfoWindow *infoWindow;

	ptr = Packet->payload;

	//user info header
	memcpy(&infoHeader, ptr, sizeof(infoHeader));
	ptr += 32; //sizeof(GR_USER_INFO_HEADER)

	infoHeader.iconID = wxUINT32_SWAP_ON_LE(infoHeader.iconID);
	infoHeader.idleSince = wxUINT32_SWAP_ON_LE(infoHeader.idleSince);
	infoHeader.lastLogin = wxUINT32_SWAP_ON_LE(infoHeader.lastLogin);
	infoHeader.lastLogout = wxUINT32_SWAP_ON_LE(infoHeader.lastLogout);
	infoHeader.userID = wxUINT32_SWAP_ON_LE(infoHeader.userID);

	//nickname
	nickname = bufToStr(ptr);
	ptr += nickname.Len() + 1;

	//real name
	realName = bufToStr(ptr);
	ptr += realName.Len() + 1;

	//email
	email = bufToStr(ptr);
	ptr += email.Len() + 1;

	//unknown 
	ptr += 12;

	//current room id
	memcpy(&roomID, ptr, sizeof(wxUint32));
	roomID = wxUINT32_SWAP_ON_LE(roomID);

	userInfo = new GRUserInfo(nickname, realName, infoHeader.userID, infoHeader.lastLogin, infoHeader.lastLogout,
		infoHeader.idleSince, infoHeader.iconID, roomID, infoHeader.location, findLobby(roomID), iconCache->findIcon(infoHeader.iconID), email);

	infoWindow = new GRUserInfoWindow(this, wxT("User Info - ")+nickname, wxDefaultPosition, wxDefaultSize);
	infoWindow->SetInfo(userInfo);
	infoWindow->Show(true);

}
//-----------------------------------------------------------------------------------
void GRMainWindow::premiumUserInfo(GR_PACKET *Packet)
{
	wxUint32 roomID, pictureID, memberSince;
	wxString nickname, realName, email, accountName, webSite, quote;
	wxUint16 favGame;
	wxUint8 *ptr;
	GR_USER_INFO_HEADER infoHeader;
	GRPremiumUserInfo *userInfo;
	GRPremiumUserInfoWindow *infoWindow;

	ptr = Packet->payload;

	//user info header
	memcpy(&infoHeader, ptr, sizeof(infoHeader));
	ptr += 32; //sizeof(GR_USER_INFO_HEADER)

	infoHeader.iconID = wxUINT32_SWAP_ON_LE(infoHeader.iconID);
	infoHeader.idleSince = wxUINT32_SWAP_ON_LE(infoHeader.idleSince);
	infoHeader.lastLogin = wxUINT32_SWAP_ON_LE(infoHeader.lastLogin);
	infoHeader.lastLogout = wxUINT32_SWAP_ON_LE(infoHeader.lastLogout);
	infoHeader.userID = wxUINT32_SWAP_ON_LE(infoHeader.userID);

	//nickname
	nickname = bufToStr(ptr);
	ptr += nickname.Len() + 1;

	//real name
	realName = bufToStr(ptr);
	ptr += realName.Len() + 1;

	//email
	email = bufToStr(ptr);
	ptr += email.Len() + 1;

	//unknown 
	ptr += 12;

	//current room id
	memcpy(&roomID, ptr, sizeof(wxUint32));
	roomID = wxUINT32_SWAP_ON_LE(roomID);
	ptr += sizeof(wxUint32);

	//account name
	accountName = bufToStr(ptr);
	ptr += accountName.Len() + 1;

	//member since
	memcpy(&memberSince, ptr, sizeof(wxUint32));
	memberSince = wxUINT32_SWAP_ON_LE(memberSince);
	ptr += sizeof(wxUint32);

	//webpage
	webSite = bufToStr(ptr);
	ptr += webSite.Len() + 1;

	//quote
	quote = bufToStr(ptr);
	ptr += quote.Len() + 1;

	//fav game
	memcpy(&favGame, ptr, sizeof(wxUint16));
	favGame = wxUINT16_SWAP_ON_LE(favGame);
	ptr += sizeof(wxUint16);

	//picture id
	memcpy(&pictureID, ptr, sizeof(wxUint32));
	pictureID = wxUINT32_SWAP_ON_LE(pictureID);
	ptr += sizeof(wxUint32);

	/*
	account name(char*)
	member since(uint32)
webpage(char*)
quote(char*)
fav game(uint16)
picture id(uint32)*/

	userInfo = new GRPremiumUserInfo(nickname, realName, infoHeader.userID, infoHeader.lastLogin, infoHeader.lastLogout,
		infoHeader.idleSince, infoHeader.iconID, roomID, infoHeader.location, findLobby(roomID), iconCache->findIcon(infoHeader.iconID), email,
		accountName, webSite, quote, memberSince, pictureID, findPluginByCode(favGame));

	logWindow->logText(accountName+wxT(" ")+findPluginByCode(favGame)->gameName);
	requestPicture(pictureID);
	infoWindow = new GRPremiumUserInfoWindow(this, wxT("User Info - ")+nickname, wxDefaultPosition, wxDefaultSize);
	infoWindow->m_mainWindow = this;
	infoWindow->SetInfo(userInfo);
	infoWindow->Show(true);
	currentPremiumInfoWindow = infoWindow;

}
//-----------------------------------------------------------------------------------
void GRMainWindow::requestPicture(wxUint32 pictureID)
{
	wxUint32 picture;

	picture = pictureID;
	picture = wxUINT32_SWAP_ON_LE(picture);

	sendGRPacket(GET_PREMIUM_USER_IMAGE, sizeof(wxUint32), (wxUint8*)&picture);
}
//-----------------------------------------------------------------------------------
void GRMainWindow::recvPremiumUserImage(GR_PACKET *Packet)
{
	wxUint8 *ptr;
	wxImage *img = new wxImage();
	wxMemoryInputStream *buf;

	ptr = Packet->payload + sizeof(wxUint32);

	buf = new wxMemoryInputStream(ptr,Packet->header->payloadLength - sizeof(wxUint32));
	img->LoadFile(*buf);

	if(currentPremiumInfoWindow != NULL)
	{	
		currentPremiumInfoWindow->SetImage(img);
	}	

	delete(buf);
	delete(img);
}
//-----------------------------------------------------------------------------------





