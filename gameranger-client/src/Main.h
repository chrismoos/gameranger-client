/*
GameRanger
Main.h

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

#ifndef MAIN_H
#define MAIN_H

#include "GRBaseDefs.h"
#include "GRLoginWindow.h"
#include "GRRegWindow.h"
#include "GRPrivateMessage.h"
#include "GRGameRoomWindow.h"
#include "GRChangeIconWindow.h"

class GRMainWindow;

class MainApp : public wxApp
{

	public:
		virtual bool OnInit();

		bool FirstRun();

		//Login Window
		GRLoginWindow *m_loginWindow;

		//Registration Window
		GRRegWindow *m_regWindow;

		//Main Window
		GRMainWindow *m_mainWindow;
};

BEGIN_EVENT_TABLE(GRRegWindow, wxFrame)
	EVT_WIZARD_PAGE_CHANGING(WIZARD_PAGE, GRRegWindow::changePage)
	EVT_WIZARD_CANCEL(-1, GRRegWindow::cancelWizard)
	EVT_SOCKET(SOCKET_ID, GRRegWindow::OnSocketEvent)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GRLoginWindow, wxFrame)
	EVT_BUTTON(LOGIN_BUTTON, GRLoginWindow::OnLoginButton)
	EVT_COMBOBOX(PROFILE_COMBO_BOX, GRLoginWindow::OnComboBoxSelect)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GRMainWindow, wxFrame)
	EVT_MENU(FILE_MENU_EXIT, GRMainWindow::MenuExit)
	EVT_MENU(FILE_MENU_LOGOUT, GRMainWindow::MenuLogout)
	EVT_MENU(LOGIN_MENU_ITEM, GRMainWindow::OnLoginMenu)
	EVT_MENU(CHANGE_PASSWORD_MENU_ITEM, GRMainWindow::OnChangePassword)
	EVT_MENU(REGISTER_MENU_ITEM, GRMainWindow::OnRegisterMenu)
	EVT_MENU(ABOUT_MENU_ABOUT, GRMainWindow::OnAboutMenu)
	EVT_MENU(CHANGE_NICK_MENU_ITEM, GRMainWindow::OnChangeNickMenu)
	EVT_MENU(CHANGE_NAME_MENU_ITEM, GRMainWindow::OnChangeNameMenu)
	EVT_MENU(USER_LIST_MENU_GET_INFO, GRMainWindow::OnUserListGetInfo)
	EVT_MENU(CHANGE_ICON_MENU_ITEM, GRMainWindow::OnChangeIconMenu)
	EVT_SOCKET(SOCKET_ID1, GRMainWindow::OnSocketEvent)
	EVT_COMBOBOX(COMBOBOX_ID, GRMainWindow::OnComboBoxSelect)
	EVT_TIMER(TIMER_ID, GRMainWindow::OnTimer)
	EVT_TEXT_ENTER(CHATEDIT_ID, GRMainWindow::OnChatEditEnter)
	EVT_LIST_ITEM_ACTIVATED(USERLISTBOX_ID, GRMainWindow::OnUserDoubleClick)
	EVT_LIST_ITEM_RIGHT_CLICK(USERLISTBOX_ID, GRMainWindow::OnUserRightClick)
	EVT_LIST_ITEM_ACTIVATED(GAMELIST_ID, GRMainWindow::OnGameRoomDoubleClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GRChangeIconWindow, wxFrame)
	EVT_BUTTON(LOAD_BUTTON_ID, GRChangeIconWindow::OnLoadIcon)
	EVT_BUTTON(CHANGE_ICON_BUTTON_ID, GRChangeIconWindow::OnChangeIcon)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GRPrivateMessage, wxFrame)
	EVT_TEXT_ENTER(PM_CHATEDIT_ID, GRPrivateMessage::OnChatEditEnter)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GRGameRoomWindow, wxFrame)
	EVT_TEXT_ENTER(GAMEROOM_CHATEDIT_ID, GRGameRoomWindow::OnChatEditEnter)
END_EVENT_TABLE()


#endif
