/*
GameRanger
GRRegWindow.cpp

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

#include "GRRegWindow.h"
#include "GRProfile.h"
#include "GRMainWindow.h"
#include "GRLoginWindow.h"
#include "memdebug.h"


GRRegWindow::GRRegWindow(const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) NULL, -1, title, pos, size)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create socket and register event handler
	m_socket = new wxSocketClient();
	m_socket->SetEventHandler(*this, SOCKET_ID);
	m_socket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_CONNECTION_FLAG | wxSOCKET_LOST_FLAG);
	m_socket->Notify(true);

	//Status Window
//	statusWindow = new GRConnectStatusWindow(this, wxT("Connecting to server..."), wxDefaultPosition, wxDefaultSize);

	//Log Window
	logWindow = new GRLogWindow(this, wxT("Log Window."), wxDefaultPosition, wxDefaultSize);

	logWindow->Show(false);
	statusWindow->Show(false);

	StartWizard();
	mainWindow = NULL;
}
//----------------------------------------------------------------------------
GRRegWindow::~GRRegWindow()
{
	statusWindow->Close();
	logWindow->Close();
	//if(mainWindow != NULL) mainWindow->regWindow = NULL;
	//wizard->Destroy();
}
//-----------------------------------------------------------------------------
int GRRegWindow::StartWizard()
{
	wizard = new wxWizard(this, WIZARD_PAGE, wxT("GameRanger Registration"));
	wizard->SetPageSize(wxSize(500, 300));
	
	accountInfoPage = new GRAccountInfoPage(wizard, NULL, NULL);
	personalInfoPage = new GRPersonalInfoPage(wizard, accountInfoPage, NULL);
	accountInfoPage->SetNext(personalInfoPage);

	wizard->SetTitle(wxT("GameRanger Regisration - Step 1(Account Information)"));
	accountInfoPage->SetName(wxT("dt"));
	stage = CHECK_EMAIL_STAGE;
#ifdef SHOW_LOG_WINDOW
	logWindow->Show(true);
#endif
	wizard->RunWizard(accountInfoPage);


	//wizard->Destroy();

	return 0;
}
//----------------------------------------------------------------------------
void GRRegWindow::changePage(wxWizardEvent &event)
{
	wxWizardPage *page;

	page = event.GetPage();

	if(page == accountInfoPage) 
	{
		if(accountInfoPage->emailEdit->GetValue().Len() == 0) {
			wxMessageDialog *msgDlg = new wxMessageDialog(this, wxT("Please enter a valid e-mail address."), wxT("Error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
			accountInfoPage->emailEdit->SetFocus();
			event.Veto();
		}
		else if(accountInfoPage->passwordEdit->GetValue().Len() == 0) {
			wxMessageDialog *msgDlg = new wxMessageDialog(this, wxT("Please enter a valid password."), wxT("Error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
			accountInfoPage->passwordEdit->SetFocus();
			event.Veto();
		}
		else if(emailConfirmed == true) {
			emailConfirmed = false;
		}
		else {
			stage = CHECK_EMAIL_STAGE;
			connectToServer();
			event.Veto();
		}
	}
	else if(page == personalInfoPage && event.GetDirection() == true)
	{
		if(personalInfoPage->nicknameEdit->GetValue().Len() == 0) {
			wxMessageDialog *msgDlg = new wxMessageDialog(this, wxT("Please enter a valid nickname."), wxT("Error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
			personalInfoPage->nicknameEdit->SetFocus();
			event.Veto();
		}
		else if(personalInfoPage->realnameEdit->GetValue().Len() == 0) {
			wxMessageDialog *msgDlg = new wxMessageDialog(this, wxT("Please enter a valid name."), wxT("Error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
			personalInfoPage->realnameEdit->SetFocus();
			event.Veto();
		}
		else {
			stage = REGISTER_STAGE;
			connectToServer();
			event.Veto();
		}
	}
}
//--------------------------------------------------------------------------
void GRRegWindow::cancelWizard(wxWizardEvent &event)
{		
	this->Close(true);
}
//-------------------------------------------------------------------------
void GRRegWindow::OnSocketEvent(wxSocketEvent& event)
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
void GRRegWindow::OnConnect()
{
	connecting = false;
	statusWindow->SetTitle(wxT("Connected to GameRanger server..."));
	statusWindow->statusIndicator->SetValue(2);
}
//----------------------------------------------------------------------------------
void GRRegWindow::OnUnableToConnect()
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
void GRRegWindow::OnDisconnect()
{
	
}
//-----------------------------------------------------------------------------
void GRRegWindow::OnDataAvailable()
{
	GR_PACKET_HEADER pckHeader;
	GR_PACKET Packet;
	wxUint8 *payload;

	//read in header
	m_socket->Read(&pckHeader, sizeof(pckHeader));

	//endian conversion
	pckHeader.command = ntohl(pckHeader.command);
	pckHeader.payloadLength = ntohl(pckHeader.payloadLength);

	//read in payload
	payload = new wxUint8[pckHeader.payloadLength];
	m_socket->Read(payload, pckHeader.payloadLength);

	//put everything into our packet structure
	Packet.header = &pckHeader;
	Packet.payload = payload;

	

	logWindow->logText(wxString::Format(wxT("Command: %08X\nPayload Length: %d"), pckHeader.command, pckHeader.payloadLength));
	logWindow->logData(Packet.payload, Packet.header->payloadLength);


	handlePacket(&Packet);

	delete[] payload;
}
//----------------------------------------------------------------------------
void GRRegWindow::handlePacket(GR_PACKET *Packet)
{
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
			if(stage == CHECK_EMAIL_STAGE) checkEmail(Packet);
			else if(stage == REGISTER_STAGE) registerAccount(Packet);
		break;

		case CONFIRM_ACCOUNT_ACK:
			if(stage == CHECK_EMAIL_STAGE) checkEmailAck(Packet);
			else if(stage == REGISTER_STAGE) createAccountAck(Packet);
		break;
	}
}
//------------------------------------------------------------------------------
void GRRegWindow::clientVerify(GR_PACKET *Packet)
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
	authAckPacket.srvVerifyCode = htonl(65535);

	m_verifyCode = authAckPacket.verifyCode;

	//logWindow->logData((wxUint8*)&authAckPacket, sizeof(authAckPacket));

	sendGRPacket(CLIENT_VERIFY_ACK, sizeof(authAckPacket), (wxUint8*)&authAckPacket);

	statusWindow->SetTitle(wxT("Checking versions..."));
	statusWindow->statusIndicator->SetValue(3);
}
//------------------------------------------------------------------------------
void GRRegWindow::sendGRPacket(wxUint32 command, wxUint32 payloadLength, wxUint8 *payload)
{
	GR_PACKET_HEADER header;
	wxUint8 *packet;

	header.command = htonl(command);
	header.payloadLength = htonl(payloadLength);

	packet = new wxUint8[sizeof(header)+payloadLength];

	memcpy(packet, &header, sizeof(header));
	memcpy(packet+sizeof(header), payload, payloadLength);

	logWindow->logData(packet, sizeof(header)+payloadLength);

	m_socket->Write(packet, sizeof(header)+payloadLength);

	delete[] packet;
}
//--------------------------------------------------------------------------------
void GRRegWindow::connectToServer()
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
	emailConfirmed = false;
	m_socket->SetFlags(wxSOCKET_WAITALL);
	m_socket->Connect(addr, false);

	/*wxUint32 test, temp;

	test = 0;

	temp = htonl(encryptVerifyCode(test));

	logWindow->logData((wxUint8*)&temp, 4);*/


}
//----------------------------------------------------------------------------
void GRRegWindow::checkEmail(GR_PACKET *Packet)
{
	statusWindow->SetTitle(wxT("Confirming E-Mail address..."));

	sendGRPacket(CONFIRM_EMAIL_ADDRESS, accountInfoPage->emailEdit->GetValue().Len()+1, (wxUint8*)(const char*)accountInfoPage->emailEdit->GetValue().mb_str());
}
//---------------------------------------------------------------------------
void GRRegWindow::checkEmailAck(GR_PACKET *Packet)
{
	GR_CONFIRM_ACCOUNT_ACK_PACKET *ackPacket;
	statusWindow->statusIndicator->SetValue(6);

	ackPacket = (GR_CONFIRM_ACCOUNT_ACK_PACKET*)Packet->payload;

	ackPacket->responseCode = ntohl(ackPacket->responseCode);

	switch(ackPacket->responseCode)
	{
		case 0: //email available
			statusWindow->SetTitle(wxT("E-Mail Address confirmed."));
			emailConfirmed = true;
			wizard->ShowPage(personalInfoPage, true);
			wizard->SetTitle(wxT("GameRanger Regisration - Step 2(Personal Information)"));
		break;

		case 1: //email unavailable
			wxMessageDialog *msgDlg = new wxMessageDialog(this, wxT("The E-Mail address you have chose is already taken. Please choose another one and try again."), wxT("Registration Error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
			accountInfoPage->emailEdit->SetFocus();
		break;
	}
	statusWindow->Show(false);

	

	m_socket->Close();
}
//---------------------------------------------------------------------------
void GRRegWindow::registerAccount(GR_PACKET *Packet)
{
	GR_CREATE_ACCOUNT_HEADER accountHeader;
	char idAndMac[10];
	wxUint32 checksum;
	wxUint8 *payload;
	wxUint8 *accountPacket;
	wxUint32 pos = 0, len = 0;
	wxUint32 previousID = 0;
	wxUint32 x;
	statusWindow->SetTitle(wxT("Registering account..."));

	idAndMac[0] = 0;
	idAndMac[1] = 0;
	idAndMac[2] = 0;
	idAndMac[3] = 0;


	srand(time(NULL));
	//make us a little random mac address
	idAndMac[4] = 0;
	idAndMac[5] = 3;
	idAndMac[6] = (wxUint8)rand()%255;
	idAndMac[7] = (wxUint8)(rand()%255);
	idAndMac[8] = (wxUint8)(rand()%255);
	idAndMac[9] = (wxUint8)(rand()%255);

	memcpy(macAddy, idAndMac+4, 6);

	for(x = 4; x < 10; x++) idAndMac[x] ^= 0x77; //xor each byte by 0x77 

	checksum = Algorithm((char*)&m_verifyCode, (char*)&idAndMac+4, previousID, NULL, 1);

	accountHeader.checksum = htonl(checksum); //oh ya
	accountHeader.padding = htonl(0);
	if(personalInfoPage->profanityFilter->IsChecked()) accountHeader.profanityFilter = htonl(0);
	else accountHeader.profanityFilter = htonl(4);
	accountHeader.iconID = htonl(0);

	len += accountInfoPage->emailEdit->GetValue().Len() + 1;
	len += accountInfoPage->passwordEdit->GetValue().Len() + 1;
	len += personalInfoPage->nicknameEdit->GetValue().Len() + 1;
	len += personalInfoPage->realnameEdit->GetValue().Len() + 1;

	payload = new wxUint8[len];

	//copy email, password, nickname, and real name

	//email
	strcpy((char*)payload, (const char*)accountInfoPage->emailEdit->GetValue().mb_str());
	pos += accountInfoPage->emailEdit->GetValue().Len() + 1;

	//password
	strcpy((char*)payload+pos, (const char*)accountInfoPage->passwordEdit->GetValue().mb_str());
	pos += accountInfoPage->passwordEdit->GetValue().Len() + 1;

	//nickname
	strcpy((char*)payload+pos, (const char*)personalInfoPage->nicknameEdit->GetValue().mb_str());
	pos += personalInfoPage->nicknameEdit->GetValue().Len() + 1;

	//real name
	strcpy((char*)payload+pos, (const char*)personalInfoPage->realnameEdit->GetValue().mb_str());
	pos += personalInfoPage->realnameEdit->GetValue().Len() + 1;


	//form complete packet
	accountPacket = new wxUint8[len+sizeof(accountHeader)+10];

	memcpy(accountPacket, idAndMac, 10);
	memcpy(accountPacket+10, &accountHeader, sizeof(accountHeader));
	memcpy(accountPacket+10+sizeof(accountHeader), payload, len);

	//logWindow->logData(accountPacket, len+sizeof(accountHeader)+10);
	sendGRPacket(CREATE_ACCOUNT,  len+sizeof(accountHeader)+10, accountPacket);
	
	delete[] accountPacket;
}
//----------------------------------------------------------------------------
void GRRegWindow::createAccountAck(GR_PACKET *Packet)
{
	GR_CREATE_ACCOUNT_ACK_PACKET *ackPacket;
	wxMessageDialog *msgDlg = NULL;

	statusWindow->statusIndicator->SetValue(6);
	
	ackPacket = (GR_CREATE_ACCOUNT_ACK_PACKET*)Packet->payload;
	ackPacket->responseCode = ntohl(ackPacket->responseCode);

	switch(ackPacket->responseCode)
	{
		case 3: //account created
			wizard->Destroy();
			msgDlg = new wxMessageDialog(this, wxT("Your account has been successfully created, please check your e-mail to activate your account."), wxT("Account Created"), wxOK);
			msgDlg->ShowModal();
			delete(msgDlg);
			writePrefs();
			this->Destroy();
		break;

		case 0: //account creation denied
			msgDlg = new wxMessageDialog(this, wxT("Your account creation was denied, this shouldn't normally happen."), wxT("Account Creation Error"), wxICON_ERROR);
			msgDlg->ShowModal();
			delete(msgDlg);
			wizard->ShowPage(accountInfoPage, true);
		break;
	}
	statusWindow->Show(false);
}
//--------------------------------------------------------------------------
void GRRegWindow::writePrefs()
{
	GRProfile *profile = new GRProfile();

	profile->email = accountInfoPage->emailEdit->GetValue();
	profile->nickname = personalInfoPage->nicknameEdit->GetValue();
	profile->realname = personalInfoPage->realnameEdit->GetValue();
	memcpy(profile->macAddress, macAddy, 6);
	profile->grID = 0;

	profile->Write();

	delete(profile);
}
//----------------------------------------------------------------------------

//============================================================================
//Account Info Page
//=============================================================================

GRAccountInfoPage::GRAccountInfoPage(wxWizard *parent, wxWizardPage* prev, wxWizardPage* next)
	: wxWizardPageSimple(parent, prev, next)
{
	//Main Sizer
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	//Panel
	wxPanel *panel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize);

	//flex grid sizer
	wxFlexGridSizer *gridSizer = new wxFlexGridSizer(2, 2, 10, 10); 


	/* --------------- Create Email and Password fields and static texts ---------- */
	emailEdit = new wxTextCtrl(panel, -1, wxT(""), wxDefaultPosition, wxSize(150, 20));
	emailEdit->SetMaxLength(32);
	passwordEdit = new wxTextCtrl(panel, -1, wxT(""), wxDefaultPosition, wxSize(150, 20), wxTE_PASSWORD);
	passwordEdit->SetMaxLength(32);
	wxStaticText *emailStatic = new wxStaticText(panel, -1, wxT("E-Mail Address: "));
	wxStaticText *passStatic = new wxStaticText(panel, -1, wxT("Password: "));

	gridSizer->Add(emailStatic, 0, wxALIGN_CENTER);
	gridSizer->Add(emailEdit, 0, wxALIGN_CENTER);
	gridSizer->Add(passStatic, 0, wxALIGN_CENTER);
	gridSizer->Add(passwordEdit, 0, wxALIGN_CENTER);

	
	mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 120);

	//Adjust window
	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(panel);
	mainSizer->SetSizeHints(this);
}
//-----------------------------------------------------------------------------

//===============================================================================
//Personal Info Page
//================================================================================

GRPersonalInfoPage::GRPersonalInfoPage(wxWizard *parent, wxWizardPage* prev, wxWizardPage* next)
	: wxWizardPageSimple(parent, prev, next)
{

	//Main Sizer
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	//Panel
	wxPanel *panel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize);

	//flex grid sizer
	wxFlexGridSizer *gridSizer = new wxFlexGridSizer(2, 2, 10, 10); 


	/* --------------- Create Email and Password fields and static texts ---------- */
	nicknameEdit = new wxTextCtrl(panel, -1, wxT(""), wxDefaultPosition, wxSize(150, 20));
	nicknameEdit->SetMaxLength(32);
	realnameEdit = new wxTextCtrl(panel, -1, wxT(""), wxDefaultPosition, wxSize(150, 20));
	realnameEdit->SetMaxLength(32);
	wxStaticText *nickStatic = new wxStaticText(panel, -1, wxT("Nickname: "));
	wxStaticText *realnameStatic = new wxStaticText(panel, -1, wxT("Real Name: "));

	profanityFilter = new wxCheckBox(panel, -1, wxT("Filter Profanity"), wxDefaultPosition, wxDefaultSize);

	gridSizer->Add(nickStatic, 0, wxALIGN_CENTER);
	gridSizer->Add(nicknameEdit, 0, wxALIGN_CENTER);
	gridSizer->Add(realnameStatic, 0, wxALIGN_CENTER);
	gridSizer->Add(realnameEdit, 0, wxALIGN_CENTER);

	
	mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxTOP | wxRIGHT | wxLEFT, 120);
	mainSizer->Add(profanityFilter, 0, wxALIGN_CENTER | wxALL, 10);
	//Adjust window
	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(panel);
	mainSizer->SetSizeHints(this);
}
//--------------------------------------------------------------------------------