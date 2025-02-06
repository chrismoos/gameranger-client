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
#include "GRLogger.h"
#include "GRPluginManager.h"

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
class GRFindPlayerWindow;

using namespace std;

//sort listbox
int wxCALLBACK wxListCompareFunction(long item1, long item2, long sortData);

class GRMainWindow : public wxFrame
{
private:
	GRLogger *logger;

	/* Plugin Manager */
	GRPluginManager *pluginManager;
public:
	/* Destructor */
	~GRMainWindow();

	/* Constructor */
	GRMainWindow(const wxString &title, const wxPoint &pos, const wxSize &size);

	/* Window Specific Stuff */
	void setupWindow();
	void createControls();

	/* Controls */
	wxMenuBar *menuBar;
	wxMenu *accountsMenu;
	wxMenu *fileMenu;
	wxMenu *aboutMenu;
	wxMenu *hostMenu;
	wxMenu *optionsMenu;
	wxTextCtrl *chatTextCtrl;
	wxTextCtrl *chatEditField;
	wxListView *gameRoomList;
	wxNotebookPage *gameRoomPage;
	wxImageList *imgList;
	wxImageList *iconImgList;
	wxStatusBar *statusBar;
	wxMenu *popupMenu;
	wxMenu *usersMenu;
	wxMenu *gameListMenu;

	/* Lobby Combo Box */
	wxComboBox *lobbyComboBox;
	void clearLobbyComboBox();
	void addLobbyToComboBox(GRLobby *lobby);
	void setCurrentLobbyInComboBox(GRLobby *lobby);
	void updateLobbyInComboBox(GRLobby *lobby);
	int getLobbyComboIndex(GRLobby *lobby);

	/* User List Box/Other user stuff */
	wxListCtrl *userListBox;
	void clearUserListBox();
	void displayUserJoinedRoom(GRUser *user);
	void displayUserJoinedGR(GRUser *user);
	void displayUserLeftRoom(GRUser *user);
	void displayUserLeftGR(GRUser *user, wxString reason);
	void displayUserLeftGR(GRUser *user);
	void addUserToListBox(GRUser *user);
	void removeUserFromListBox(GRUser *user);
	void updateUserInListBox(GRUser *user);
	int getUserListBoxIndex(GRUser *user);
	void updateUserIcon(GRUser *user);

	/* Game Room List */
	void clearGameRooms();
	void displayGameRoomOpened(GRGameRoom *gameRoom);
	void displayGameRoomClosed(GRGameRoom *gameRoom);
	void addGameRoomToListView(GRGameRoom *gameRoom);
	void removeGameFromListView(GRGameRoom *gameRoom);
	void updateGameRoomInListView(GRGameRoom *gameRoom);
	wxInt32 getGameItemIndex(GRGameRoom *room);

	/* Enable/Disable Menus */
	void enableMenus();
	void disableMenus();

	/* Image List */
	void setupImageLists();
	void setupIconImageList();
	void setupPluginImageList();

	/* Plugin Load Event */
	void OnPluginLoadDone(wxCommandEvent &evt);

	/* Icon Cache Event */
	void OnLoadCacheDone(wxCommandEvent &evt);

	//Cleanup
	void cleanupGR();

	

	//Windows
	GRLoginWindow *loginWindow;
	GRLogWindow *logWindow;
	GRConnectStatusWindow *statusWindow;
	GRRegWindow *regWindow;

	//Events
	void OnWindowClose(wxCloseEvent &event);
	void MenuExit(wxCommandEvent &event);
	void OnHostRoomMenu(wxCommandEvent &event);
	void MenuLogout(wxCommandEvent &event);
	void OnComboBoxSelect(wxCommandEvent &event);
	void OnTimer(wxTimerEvent &evt);
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
	void OnFindPlayerMenu(wxCommandEvent &event);
	void OnChangeMyGamesMenu(wxCommandEvent &event);
	void OnToggleTimestamp(wxCommandEvent &event);


	//User List Box Helpers
	wxInt32 getUserItemIndex(GRUser *user);

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
	void checkEmail(GR_PACKET *Packet);
	void checkEmailAck(GR_PACKET *Packet);
	wxUint32 m_verifyCode;

	//Login
	void Login(char *email, char *password);

	//My Info
	wxUint32 myUserID;
	wxString myNickname;

	//Helper Functions
	wxString bufToStr(wxUint8 *text);

	//Lobby Functions/Variables
	vector <GRLobby*> Lobbies;
	GRLobby *currentLobby;
	void lobbyUserList(GR_PACKET *Packet);
	GRLobby *findLobby(wxUint32 lobbyID);
	void setListInfo(GRUser *User);
	void joinLobby(GRLobby *lobby);

	//Game Rooms
	vector <GRGameRoom*> GameRooms;
	void addGameRoom(wxUint32 gameRoomID, wxUint32 gameCode, wxUint32 maxPlayers, wxUint32 currentPlayers, wxString ip, wxUint8 locked, wxString description, wxUint32 grID, wxUint32 iconID, wxUint32 status, wxString host, bool announce);
	void playerJoinedGameRoom(GR_PACKET *Packet);
	GRGameRoom *findGameRoom(wxUint32 id);
	void updateGameRoomPlayerCountString(GRGameRoom *room);
	void playerLeftGameRoom(GR_PACKET *Packet);
	void gameRoomUserList(GR_PACKET *Packet);
	void gameRoomStatusChanged(GR_PACKET *Packet);
	void setGameRoomListInfo(GRGameRoom *gameRoom);
	void gameRoomLaunched(GR_PACKET *Packet);
	bool gameRoomWillClose;

	//Plugins
	vector <GRPlugin*> Plugins;
	void parseGamesListForUser(GRUser *user, wxUint8 *buf);
	wxUint8 *makePluginsList();
	void clearGamesListMenu();
	void makeGameListMenu(wxListCtrl *list, wxMenu *gameMenu, int index);
	wxUint8 *makeGameList(vector <GRPlugin*> list);

	//Banners
	void saveBanner(wxUint8 *data, wxUint32 length, wxUint32 bannerID);
	void appBanner(GR_PACKET *Packet);

	//Icon Cache
	GRIconCache *iconCache;

	//Private Messages
	vector <GRPrivateMessage*> privateMessages;
	GRPrivateMessage *findPrivateMessageByID(wxUint32 userID);
	void receivedPrivateMessage(GR_PACKET *Packet);

	//Game Room Windows
	vector <GRGameRoomWindow*> gameRoomWindows;
	GRGameRoomWindow *currentGameRoom;
    wxUint32 currentRoomID;

	//User Info
	GRPremiumUserInfoWindow *currentPremiumInfoWindow;
	bool roomWantedInfo;

	//Log Packets
	void logPacket(wxUint8 *buf, wxUint32 len);
	wxUint8 *createRawPacket(wxUint32 command, wxUint32 len, wxUint8 *payload);


	/* Timestamps */
	bool showTimestamps;
	wxMenuItem *timestampItem;

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
	CHANGE_ICON_MENU_ITEM,
	FIND_PLAYER_MENU,
	GAMES_LIST_MENU,
	CHANGE_MY_GAMES_ITEM,
	HOST_MENU_ITEM,
	TIMESTAMP_TOGGLE,
	GRMAINWINDOW_ID
};

#endif

