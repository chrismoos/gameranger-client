/*
GameRanger
GRRegWindow.h

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

#ifndef GRREGWINDOW_H
#define GRREGWINDOW_H

#include "GRBaseDefs.h"
#include "GRConnectStatusWindow.h"
#include "GRLogWindow.h"
#include "GRSecurity.h"

class GRMainWindow;

class GRAccountInfoPage : public wxWizardPageSimple
{
	public:
		GRAccountInfoPage(wxWizard *parent, wxWizardPage* prev, wxWizardPage* next);

		//GUI Controls
		wxTextCtrl *emailEdit;
		wxTextCtrl *passwordEdit;
};

class GRPersonalInfoPage : public wxWizardPageSimple
{
	public:
		GRPersonalInfoPage(wxWizard *parent, wxWizardPage* prev, wxWizardPage* next);

		//GUI Controls
		wxTextCtrl *nicknameEdit;
		wxTextCtrl *realnameEdit;

		wxCheckBox *profanityFilter;
};

class GRRegWindow : public wxFrame
{

	public:
		GRRegWindow(const wxString &title, const wxPoint &pos, const wxSize &size);
		~GRRegWindow();

		//Start Wizard
		int StartWizard();

		//Connection window
		GRConnectStatusWindow *statusWindow;

		//Log Window
		GRLogWindow *logWindow;

		//Main Window
		GRMainWindow *mainWindow;

		//Pages
		GRAccountInfoPage *accountInfoPage;
		GRPersonalInfoPage *personalInfoPage;

		//Events
		void changePage(wxWizardEvent &event);
		void cancelWizard(wxWizardEvent &event);

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

		//Register Function
		void registerAccount(GR_PACKET *Packet);
		void createAccountAck(GR_PACKET *Packet);

		//Preferences
		void writePrefs();

		//Mac Address
		wxUint8 macAddy[6];

		bool emailConfirmed;

		wxWizard *wizard;
		DECLARE_EVENT_TABLE()
};

enum 
{
	WIZARD_PAGE = wxID_HIGHEST + 1,
	SOCKET_ID,
	CHECK_EMAIL_STAGE,
	REGISTER_STAGE
};


#endif
