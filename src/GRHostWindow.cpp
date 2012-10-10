/*
GameRanger
GRHostWindow.cpp

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

#include "GRHostWindow.h"
#include "GRMainWindow.h"
#include "GRGameRoom.h"
#include "GRUser.h"
#include "GRIconCache.h"
#include "GRIcon.h"
#include "GRPlugin.h"
#include "GRLogWindow.h"
#include "GRPrivateMessage.h"
#include "memdebug.h"

GRHostWindow::GRHostWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *)parent, -1, title, pos, size)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create Controls
	createControls();

	//Center window
	CentreOnScreen();
}
//-----------------------------------------------------------------------------------
GRHostWindow::~GRHostWindow()
{

	
}
//--------------------------------------------------------------------------------
void GRHostWindow::createControls()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	/* grid sizer */
	wxFlexGridSizer *gridSizer = new wxFlexGridSizer(4, 2, 10, 5); 

	/* static texts */
	wxStaticText *gameText = new wxStaticText(this, -1, wxT("Game: "),
		wxDefaultPosition, wxDefaultSize);
	wxStaticText *playersText = new wxStaticText(this, -1, wxT("Players: "), 
		wxDefaultPosition, wxDefaultSize);
	wxStaticText *roomTitleText = new wxStaticText(this, -1, wxT("Description: "),
		wxDefaultPosition, wxDefaultSize);
	wxStaticText *passText = new wxStaticText(this, -1, wxT("Password: "), 
		wxDefaultPosition, wxDefaultSize);

	/* game list box */
	gameCombo = new wxComboBox(this, HOST_GAME_COMBO_LIST, wxT(""), wxDefaultPosition,
		wxSize(200,20), NULL, wxCB_READONLY | wxCB_SORT);

	/* players combo box */
	playersCombo = new wxComboBox(this, PLAYERS_GAME_COMBO_LIST, wxT(""), wxDefaultPosition,
		wxSize(50,20), NULL, wxCB_READONLY);

	/* description text box */
	descText = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition,
		wxSize(200,20));

	/* password text box */
	passTextBox = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition,
		wxSize(200,20));

	/* add stuff to grid */
	gridSizer->Add(gameText);
	gridSizer->Add(gameCombo);
	gridSizer->Add(playersText);
	gridSizer->Add(playersCombo);
	gridSizer->Add(roomTitleText);
	gridSizer->Add(descText);
	gridSizer->Add(passText);
	gridSizer->Add(passTextBox);

	/* late joiners radio box */
	lateJoinersCheck = new wxCheckBox(this, -1, wxT("No late joiners "),
		wxDefaultPosition, wxDefaultSize);

	/* button */
	wxButton *button = new wxButton(this, HOST_BUTTON_ID, wxT("Host"),
		wxDefaultPosition, wxDefaultSize);

	/* add to main sizer */
	sizer->Add(gridSizer, 0, wxALL | wxALIGN_CENTER, 15);
	sizer->Add(lateJoinersCheck, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER, 15);
	sizer->Add(button, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER, 15);


	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(sizer);
	sizer->Fit(this);
	sizer->SetSizeHints(this);
}
//------------------------------------------------------------------------------------
void GRHostWindow::initListBoxes()
{
	int x;
	GRPlugin *plugin;

	for(x = 0; x < mainWindow->Plugins.size(); x++) {
		plugin = mainWindow->Plugins[x];
		if(plugin->gameCode != 0xffffffff)
			gameCombo->Append(plugin->gameName, (void*)plugin);
	}

	for(x = 1; x <= 64; x++) {
		playersCombo->Append(wxString::Format(wxT("%d"),x));
	}
}
//---------------------------------------------------------------------------------------
void GRHostWindow::OnHostButton(wxCommandEvent &evt)
{
	wxUint8 *payload = NULL;
	wxUint32 len = 0;
	GR_HOST_GAME_ROOM hostHeader;
	GRPlugin *plugin = NULL;
	int pos = 0;
	char lateJoiners[1];
	char nulls[1] = {0};

	if(gameCombo->GetSelection() == -1) return;
	plugin = (GRPlugin*)gameCombo->GetClientData(gameCombo->GetSelection());
	if(plugin == NULL) return;

	len += sizeof(GR_HOST_GAME_ROOM);
	len += 1; //unknown
	len += descText->GetValue().Length() + 1;
	len += passTextBox->GetValue().Length() + 1;
	len += 1; //premium only

	payload = new wxUint8[len];

	/* host header */
	hostHeader.gameID = wxUINT32_SWAP_ON_LE(plugin->gameCode);
	hostHeader.unknown = wxUINT32_SWAP_ON_LE(0xffffffff);
	hostHeader.maxPlayers = wxUINT32_SWAP_ON_LE(atoi((const char*)playersCombo->GetValue().mb_str()));
	hostHeader.lateJoiners = wxUINT32_SWAP_ON_LE(0);

	if(lateJoinersCheck->IsChecked())
		lateJoiners[0] = 0;
	else
		lateJoiners[0] = 1;

	memcpy(payload, &hostHeader, sizeof(hostHeader));
	pos += sizeof(hostHeader);

	/* null byte */
	memcpy(payload+pos, lateJoiners, 1);
	pos++;

	/* description */
	memcpy(payload+pos, (const char*)descText->GetValue().mb_str(), descText->GetValue().Length()+1);
	pos += descText->GetValue().Length()+1;

	/* pass */
	memcpy(payload+pos, (const char*)passTextBox->GetValue().mb_str(), passTextBox->GetValue().Length()+1);
	pos += passTextBox->GetValue().Length()+1;

	/* premium only */
	memcpy(payload+pos, nulls, 1);
	pos++;

	mainWindow->sendGRPacket(HOST_GAME_ROOM, len, payload);
	this->Close();

	delete[] payload;
}
//---------------------------------------------------------------------------------------
void GRHostWindow::OnGameComboBoxSelect(wxCommandEvent &evt)
{

}
//---------------------------------------------------------------------------------

