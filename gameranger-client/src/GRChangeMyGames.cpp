/*
GameRanger
GRChangeMyGames.cpp

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

#include "GRChangeMyGames.h"
#include "GRBaseDefs.h"
#include "GRMainWindow.h"
#include "GRProfile.h"
#include "GRIcon.h"
#include "GRUser.h"
#include "GRLogWindow.h"
#include "GRPlugin.h"
#include "memdebug.h"

GRChangeMyGames::GRChangeMyGames(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) parent, -1, title, pos, size, wxSYSTEM_MENU | wxCAPTION)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create controls
	createControls();

	//Center window
	CentreOnScreen();

	//null it
	m_mainWindow = NULL;

}
//------------------------------------------------------------------------------------
GRChangeMyGames::~GRChangeMyGames()
{
	if(m_mainWindow != NULL) m_mainWindow->searchWindow = NULL;
}
//-------------------------------------------------------------------------------------
void GRChangeMyGames::createControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

	m_listBox = new wxCheckListBox(this, -1, wxDefaultPosition, wxSize(350, 400),
		0, NULL, wxLB_SORT);
	m_saveButton = new wxButton(this, SAVE_GAME_LIST_BUTTON_ID, wxT("Save Game List"));
	wxButton *checkAllButton = new wxButton(this, CHECK_ALL_BUTTON_ID, wxT("Check All"));
	wxButton *uncheckAllButton = new wxButton(this, UNCHECK_ALL_BUTTON_ID, wxT("Uncheck All"));

	//bottom sizer
	bottomSizer->Add(m_saveButton, 0, wxALIGN_CENTER);
	bottomSizer->Add(checkAllButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER, 2);
	bottomSizer->Add(uncheckAllButton, 0, wxLEFT | wxALIGN_CENTER, 2);

	mainSizer->Add(m_listBox, 0, wxALL, 7);
	mainSizer->Add(bottomSizer, 0, wxALIGN_CENTER | wxBOTTOM | wxRIGHT | wxLEFT, 7);

	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);
}
//--------------------------------------------------------------------------------------
void GRChangeMyGames::OnSaveGameList(wxCommandEvent &event)
{
	vector <GRPlugin*> plugins;
	int x, y;
	wxUint8 *buf;
	wxString str;

	for(x = 0; x < m_listBox->GetCount(); x++)
	{
		if(!m_listBox->IsChecked(x)) continue;
		str = m_listBox->GetString(x);
		for(y = 0; y < m_mainWindow->Plugins.size(); y++)
		{
			if(m_mainWindow->Plugins[y]->gameName == str)
			{
				plugins.push_back(m_mainWindow->Plugins[y]);
				break;
			}
		}
	}

	buf = m_mainWindow->makeGameList(plugins);

	m_mainWindow->logWindow->logData(buf, *(buf)+1);
	if(m_mainWindow->currentProfile != NULL)
	{
		if(m_mainWindow->currentProfile->gamesList != NULL) delete[] m_mainWindow->currentProfile->gamesList;
		m_mainWindow->currentProfile->gamesList = new wxUint8[*(buf)+1];
		memcpy(m_mainWindow->currentProfile->gamesList, buf, *(buf)+1);
		m_mainWindow->currentProfile->Write();
	}

	delete[] buf;

	this->Close();
}
//---------------------------------------------------------------------------------------
void GRChangeMyGames::populateListBox()
{
	int x, y;
	GRPlugin *plugin;
	wxString str;
	GRUser *user;
	if(m_mainWindow == NULL) return;

	for(x = 0; x < m_mainWindow->Plugins.size(); x++)
	{
		plugin = (GRPlugin*)m_mainWindow->Plugins[x];
		if(plugin->gameCode == 0xffffffff) continue;
		if(plugin->gameCode == 0) continue;
		m_listBox->Append(plugin->gameName);
	}

	if(m_mainWindow->currentProfile == NULL) return;

	//Check games we have in profile
	user = new GRUser();
	m_mainWindow->parseGamesListForUser(user, m_mainWindow->currentProfile->gamesList);
	for(x = 0; x < user->gamesList.size(); x++)
	{
		for(y = 0; y < m_listBox->GetCount(); y++)
		{
			str = m_listBox->GetString(y);
			if(str == user->gamesList[x]->gameName)
			{
				m_listBox->Check(y, true);
				break;
			}
		}
	}
	delete(user);
}
//----------------------------------------------------------------------------------------
void GRChangeMyGames::OnCheckAll(wxCommandEvent &event)
{
	int x;
	for(x = 0; x < m_listBox->GetCount(); x++)
	{
		m_listBox->Check(x, true);
	}
}
//---------------------------------------------------------------------------------------
void GRChangeMyGames::OnUncheckAll(wxCommandEvent &event)
{
	int x;
	for(x = 0; x < m_listBox->GetCount(); x++)
	{
		m_listBox->Check(x, false);
	}
}
//---------------------------------------------------------------------------------------


