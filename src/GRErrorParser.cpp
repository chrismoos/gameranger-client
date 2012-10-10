/*
GameRanger
GRErrorParser.cpp

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

#include "GRErrorParser.h"
#include "GRApplication.h"
#include "GRConnectStatusWindow.h"
#include "GRLoginWindow.h"

GRErrorParser::GRErrorParser(GRConnection *conn)
{
	this->conn = conn;
}
/*--------------------------------------------------------------*/
GRErrorParser::~GRErrorParser()
{

}
/*--------------------------------------------------------------*/
bool GRErrorParser::run(GR_PACKET *Packet)
{
	wxMessageDialog *msgDlg;

	switch(Packet->header->command)
	{
		case INVALID_LOGIN_PASSWORD:
			GRConnectStatusWindow::getInstance()->Show(false);
			invalidPassword();
		break;

		case INVALID_ACCOUNT:
			GRConnectStatusWindow::getInstance()->Show(false);
			invalidAccount();
		break;

		case GAME_ROOM_FULL:
			msgDlg = new wxMessageDialog(GRApplication::getInstance()->getMainWindow(), wxT("The game room you are trying to join is full, please try again later."), wxT("Join room error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
		break;

		case GAME_ROOM_NO_LATE_JOINERS:
			msgDlg = new wxMessageDialog(GRApplication::getInstance()->getMainWindow(), wxT("The game room you are trying to join does not allow late joiners."), wxT("Join room error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
		break;

		case GAME_ROOM_INVALID_PASSWORD:
			msgDlg = new wxMessageDialog(GRApplication::getInstance()->getMainWindow(), wxT("The game room password you specified is invalid."), wxT("Join room error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
		break;

		case PASSWORD_CHANGE_SUCCESSFUL:
			msgDlg = new wxMessageDialog(GRApplication::getInstance()->getMainWindow(), wxT("Your password was successfully changed."), wxT("Password change"), wxICON_INFORMATION);
			msgDlg->ShowModal();
			delete(msgDlg);
		break;

		case PM_ERROR_USER_IN_GAME:
			msgDlg = new wxMessageDialog(GRApplication::getInstance()->getMainWindow(), wxT("Error: The user you are trying to private message is in a game."), wxT("Private Message Error"), wxICON_INFORMATION);
			msgDlg->ShowModal();
			delete(msgDlg);
		break;

		default:
			return false;
		break;
	}

	return true;
}
/*--------------------------------------------------------------*/
void GRErrorParser::invalidPassword()
{
	wxMessageDialog *msgDlg = new wxMessageDialog(GRApplication::getInstance()->getMainWindow(), wxT("The password you entered is invaild, please revise your password and try again."), wxT("Login Error"), wxICON_ERROR);
	msgDlg->ShowModal();
	delete(msgDlg);
	GRLoginWindow::getInstance()->SetFocus();
}
/*--------------------------------------------------------------*/
void GRErrorParser::invalidAccount()
{
	wxMessageDialog *msgDlg = new wxMessageDialog(GRApplication::getInstance()->getMainWindow(), wxT("The account you specified is either invalid or hasn't been activated, please check your email and try again."), wxT("Login Error"), wxICON_ERROR);
	msgDlg->ShowModal();
	delete(msgDlg);
	GRLoginWindow::getInstance()->SetFocus();
}
/*--------------------------------------------------------------*/




