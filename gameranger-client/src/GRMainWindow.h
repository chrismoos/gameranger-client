/*
GameRanger
GRMainWindow.h

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

#ifndef GRMAINWINDOW_H
#define GRMAINWINDOW_H

#include "GRBaseDefs.h"

class GRLoginWindow;
class GRLogWindow;
class GRConnectStatusWindow;
class GRLobby;
class GRGameRoom;
class GRUser;
class GRPlugin;
class GRGameRoom;
class GRProfile;
class GRIconCache;
class GRIcon;
class GRPrivateMessage;
class GRRegWindow;
class GRGameRoomWindow;
class GRPremiumUserInfoWindow;

using namespace std;

//sort listbox
int wxCALLBACK wxListCompareFunction(long item1, long item2, long sortData);

class GRMainWindow : public wxFrame
{
public:
	GRMainWindow(const wxString &title, const wxPoint &pos, const wxSize &size);
	~GRMainWindow();

	//Controls
	wxMenuBar *menuBar;
	wxMenu *accountsMenu;
	wxMenu *fileMenu;
	wxMenu *aboutMenu;
	wxMenu *optionsMenu;
	wxComboBox *lobbyComboBox;
	wxTextCtrl *chatTextCtrl;
	wxTextCtrl *chatEditField;
	wxListCtrl *userListBox;
	wxListView *gameRoomList;
	wxNotebookPage *gameRoomPage;
	wxImageList *imgList;
	wxImageList *iconImgList;
	wxStatusBar *statusBar;
	wxMenu *popupMenu;

	//Cleanup
	void cleanupGR();

	void createControls();

	//Windows
	GRLoginWindow *loginWindow;
	GRLogWindow *logWindow;
	GRConnectStatusWindow *statusWindow;
	GRRegWindow *regWindow;

	//Events
	void MenuExit();
	void MenuLogout();
	void OnComboBoxSelect(wxCommandEvent &event);
	void OnTimer();
	void OnChatEditEnter(wxCommandEvent &event);
	void OnLoginMenu(wxCommandEvent &event);
	void OnRegisterMenu(wxCommandEvent &event);
	void OnAboutMenu(wxCommandEvent &event);
	void OnUserDoubleClick(wxListEvent& event);
	void OnGameRoomDoubleClick(wxListEvent& event);
	void OnChangeNickMenu(wxCommandEvent &event);
	void OnChangeNameMenu(wxCommandEvent &event);
	void OnChangePassword(wxCommandEvent &event);
	void OnUserRightClick(wxListEvent &event);
	void OnUserListGetInfo(wxCommandEvent &event);
	void OnChangeIconMenu(wxCommandEvent &event);

	//User List Box Helpers
	wxInt32 getUserItemIndex(GRUser *user);

	//Game Room list helpers
	wxInt32 getGameItemIndex(GRGameRoom *room);

	//Chat Text Box Help
	void addTextWithColor(wxString str, wxColour color);

	//Socket
	bool connecting;
	wxSocketClient *m_socket;
	int stage;
	void connectToServer();
	void sendGRPacket(wxUint32 command, wxUint32 payloadLength, wxUint8 *payload);
	
	//socket events
	void OnSocketEvent(wxSocketEvent& event);
	void OnConnect();
	void OnUnableToConnect();
	void OnDisconnect();
	void OnDataAvailable();

	//GR Packet parsing
	void handlePacket(GR_PACKET *Packet);

	//Auth Functions
	void clientVerify(GR_PACKET *Packet);
	void checkEmail(GR_PACKET *Packet);
	void checkEmailAck(GR_PACKET *Packet);
	wxUint32 m_verifyCode;

	//Login
	void loginToGR(GR_PACKET *Packet);
	void Login(char *email, char *password);
	void invalidPassword();
	void myUserInfo(GR_PACKET *Packet);

	//Server Message
	void serverMessage(GR_PACKET *Packet);

	//Ban 
	void banTimeLeft(GR_PACKET *Packet);

	//Invalid account
	void invalidAccount();

	//My Info
	wxUint32 myUserID;
	wxString myNickname;

	//Helper Functions
	wxString bufToStr(wxUint8 *text);

	//Lobby Functions/Variables
	vector <GRLobby*> Lobbies;
	GRLobby *currentLobby;
	void publicLobbyList(GR_PACKET *Packet);
	void lobbyUserList(GR_PACKET *Packet);
	GRLobby *findLobby(wxUint32 lobbyID);
	void setListInfo(GRUser *User);
	void joinLobby(GRLobby *lobby);

	//Game Rooms
	vector <GRGameRoom*> GameRooms;
	void gameRoomsList(GR_PACKET *Packet);
	void addGameRoom(wxUint32 gameRoomID, wxUint32 gameCode, wxUint32 maxPlayers, wxUint32 currentPlayers, wxString ip, wxUint8 locked, wxString description, wxUint32 grID, wxUint32 iconID, wxUint32 status, wxString host, bool announce);
	void gameRoomOpened(GR_PACKET *Packet);
	void gameRoomClosed(GR_PACKET *Packet);
	void playerJoinedGameRoom(GR_PACKET *Packet);
	GRGameRoom *findGameRoom(wxUint32 id);
	void updateGameRoomPlayerCountString(GRGameRoom *room);
	void playerLeftGameRoom(GR_PACKET *Packet);
	void gameRoomUserList(GR_PACKET *Packet);

	//Plugins
	void loadPlugins();
	GRPlugin *findPluginByCode(wxUint32 gameCode);
	vector <GRPlugin*> Plugins;

	//Mac OS Color Profile
	void loadColorProfile();
	wxUint8 *colorTable;

	//Test stuff
	void testPacket(wxString filename);

	//Icon Cache
	void requestMissingIconsFromUserList(wxUint32 count);
	void requestIcon(wxUint32 iconID);
	void purgeIcons();
	void userChangedIcon(GR_PACKET *Packet);
	void receivedIconData(GR_PACKET *Packet);
	GRIconCache *iconCache;

	//Private Messages
	vector <GRPrivateMessage*> privateMessages;
	GRPrivateMessage *findPrivateMessageByID(wxUint32 userID);
	void receivedPrivateMessage(GR_PACKET *Packet);

	//Game Room Windows
	vector <GRGameRoomWindow*> gameRoomWindows;
	GRGameRoomWindow *currentGameRoom;

	//User Info
	void regularUserInfo(GR_PACKET *Packet);
	void premiumUserInfo(GR_PACKET *Packet);
	void requestPicture(wxUint32 pictureID);
	GRPremiumUserInfoWindow *currentPremiumInfoWindow;
	void recvPremiumUserImage(GR_PACKET *Packet);

	//Log Packets
	void logPacket(wxUint8 *buf, wxUint32 len);
	wxUint8 *createRawPacket(wxUint32 command, wxUint32 len, wxUint8 *payload);


	char GRemail[128];
	char GRpassword[128];

	bool loggedIn;
	int lastIndex;

	//Timer
	wxTimer *timer;

	//Packet Counter
	wxUint32 packetCounter;
	wxUint32 getPacketCounterByte(wxUint32 command, wxUint32 payloadLength);
	bool writeCounter(wxUint32 command);

	wxUint32 lastCommand;
	wxUint32 lastLength;

	//Current Profile
	GRProfile *currentProfile;

	DECLARE_EVENT_TABLE()
};

enum {
	SOCKET_ID1 = wxID_HIGHEST + 30,
	FILE_MENU_EXIT,
	ABOUT_MENU_ABOUT,
	COMBOBOX_ID,
	CHATEDIT_ID,
	CHATLOG_ID,
	TIMER_ID,
	LOGIN_MENU_ITEM,
	REGISTER_MENU_ITEM,
	CHANGE_NICK_MENU_ITEM,
	CHANGE_NAME_MENU_ITEM,
	CHANGE_PASSWORD_MENU_ITEM,
	USERLISTBOX_ID,
	GAMELIST_ID,
	FILE_MENU_LOGOUT,
	USER_LIST_MENU_GET_INFO,
	CHANGE_ICON_MENU_ITEM
};

#endif

