/*
GameRanger
GRPrivateMessage.cpp

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

#include "GRPrivateMessage.h"
#include "GRMainWindow.h"

GRPrivateMessage::GRPrivateMessage(const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) NULL, -1, title, pos, size)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create controls
	createControls();

	//Null some stuff
	mainWindow = 0;

	//Center window
	CentreOnScreen();

	userID = 0;
	nickname = wxT("nick");
}
//------------------------------------------------------------------------------------
GRPrivateMessage::~GRPrivateMessage()
{
	wxUint32 x;
	vector <GRPrivateMessage*>::iterator it;
	GRPrivateMessage *msg;
	//Delete this instance from main window's pm array
	for(x = 0; x < mainWindow->privateMessages.size(); x++)
	{
		msg = mainWindow->privateMessages[x];
		if(msg == this) //found it
		{
			it = mainWindow->privateMessages.begin()+x;
			mainWindow->privateMessages.erase(it);
			break;
		}
	}
}
//-------------------------------------------------------------------------------------
void GRPrivateMessage::createControls()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	//Add chat edit field
	chatEditField = new wxTextCtrl(this, PM_CHATEDIT_ID, wxT(""), wxDefaultPosition,
		wxSize(300, 20), wxTE_PROCESS_ENTER);

	//Add chat window
	chatWindow = new wxTextCtrl(this, PM_CHATLOG_ID, wxT(""), wxDefaultPosition,
		wxSize(300, 150), wxTE_MULTILINE | wxTE_NOHIDESEL | wxTE_READONLY | wxTE_RICH2 | wxTE_AUTO_URL);

	
	chatEditField->SetFocus();

	sizer->Add(chatWindow, 0, wxALL, 5);
	sizer->Add(chatEditField, 0, wxBOTTOM | wxRIGHT | wxLEFT, 5);

	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(sizer);
	sizer->Fit(this);
	sizer->SetSizeHints(this);
}
//------------------------------------------------------------------------------------
void GRPrivateMessage::OnChatEditEnter(wxCommandEvent &event)
{
	wxUint8 *payload;
	wxUint32 len = 0;
	wxUint32 temp;

	if(chatEditField->GetValue().Len() == 0) return;
	wxString msg = chatEditField->GetValue();
	len = sizeof(wxUint32) + msg.Len() + 1;
	payload = new wxUint8[len];

	temp = wxUINT32_SWAP_ON_LE(userID);
	memcpy(payload, &temp, sizeof(wxUint32));
	strcpy((char*)payload+sizeof(wxUint32), (char*)(const char*)msg.c_str());

	mainWindow->sendGRPacket(SEND_PRIVATE_MESSAGE, len, payload);

	addTextWithColor(mainWindow->myNickname, *wxRED);
	addTextWithColor(wxT(": ") + chatEditField->GetValue() + wxT("\n"), *wxBLACK);

	chatEditField->SetValue(wxT(""));

	delete[] payload;
}
//------------------------------------------------------------------------------------
void GRPrivateMessage::addTextWithColor(wxString str, wxColour color)
{
	chatWindow->SetDefaultStyle(wxTextAttr(color, *wxWHITE));
	chatWindow->AppendText(str);
	chatWindow->SetDefaultStyle(*wxRED);

	chatWindow->SetInsertionPointEnd();

}
//-----------------------------------------------------------------------------

