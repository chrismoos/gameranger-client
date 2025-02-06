/*
GameRanger
GRFindPlayerWindow.cpp

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

#include "GRFindPlayerWindow.h"
#include "GRBaseDefs.h"
#include "GRMainWindow.h"
#include "GRUtil.h"
#include "GRApplication.h"
#include "GRConnection.h"
#include "GRIcon.h"
#include "GRLogWindow.h"
#include "memdebug.h"

/* Instance */
GRFindPlayerWindow *GRFindPlayerWindow::_instance = NULL;

GRFindPlayerWindow::GRFindPlayerWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) parent, -1, title, pos, size)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create controls
	createControls();

	//Center window
	CentreOnScreen();
}
//------------------------------------------------------------------------------------
GRFindPlayerWindow::~GRFindPlayerWindow()
{

}
//-------------------------------------------------------------------------------------
GRFindPlayerWindow *GRFindPlayerWindow::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRFindPlayerWindow(NULL, wxT("Find user"), wxDefaultPosition, wxDefaultSize);
	}
	return _instance;
}
//-------------------------------------------------------------------------------------
GRFindPlayerWindow *GRFindPlayerWindow::getInstance(wxFrame *parent)
{
	if(_instance == NULL) {
		_instance = new GRFindPlayerWindow(parent, wxT("Find user"), wxDefaultPosition, wxDefaultSize);
	}
	return _instance;
}
//-------------------------------------------------------------------------------------
void GRFindPlayerWindow::createControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticBox *staticBox = new wxStaticBox(this, -1, wxT("Search Type"));
	wxStaticBox *resultsStaticBox = new wxStaticBox(this, -1, wxT("Search Results"));
	wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(staticBox, wxVERTICAL);
	resultsSizer = new wxStaticBoxSizer(resultsStaticBox, wxVERTICAL);
	wxBoxSizer *mainTopSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);


	//results list box
	m_resultsList = new wxListView(this, RESULTS_LIST_ID, wxDefaultPosition,
		wxSize(200, 150), wxLC_REPORT | wxLC_SORT_ASCENDING | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT);

	m_resultsList->InsertColumn(0, wxT("Account"), wxLIST_FORMAT_LEFT, 75);
	m_resultsList->InsertColumn(1, wxT("Nickname"), wxLIST_FORMAT_LEFT, 128);
	


	//Static Texts
	wxStaticText *accountText = new wxStaticText(this, -1, wxT("Account Name:"));
	wxStaticText *nickText = new wxStaticText(this, -1, wxT("Nickname:"));
	wxStaticText *emailText = new wxStaticText(this, -1, wxT("E-Mail:"));
	wxStaticText *userIDText = new wxStaticText(this, -1, wxT("User ID:"));

	//text boxes
	m_accountValue = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize(125,20));
	m_nickValue = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize(125,20));
	m_emailValue = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize(125,20));
	m_userIDValue = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize(125,20));

	//button
	m_findButton = new wxButton(this, SEARCH_BUTTON_ID, wxT("Search"));

	//left sizer
	leftSizer->Add(accountText, 0, wxBOTTOM | wxRIGHT, 12);
	leftSizer->Add(nickText, 0, wxBOTTOM | wxRIGHT, 12);
	leftSizer->Add(emailText, 0, wxBOTTOM | wxRIGHT, 12);
	leftSizer->Add(userIDText, 0, wxBOTTOM | wxRIGHT, 12);

	//right sizer
	rightSizer->Add(m_accountValue, 0, wxBOTTOM | wxLEFT, 5);
	rightSizer->Add(m_nickValue, 0, wxBOTTOM | wxLEFT, 5);
	rightSizer->Add(m_emailValue, 0, wxBOTTOM | wxLEFT, 5);
	rightSizer->Add(m_userIDValue, 0, wxBOTTOM | wxLEFT, 5);

	//Add to our main top sizer thing
	mainTopSizer->Add(leftSizer, 0, wxLEFT | wxTOP | wxBOTTOM, 5);
	mainTopSizer->Add(rightSizer, 0, wxRIGHT | wxTOP | wxBOTTOM, 5);

	//add to static box sizer
	boxSizer->Add(mainTopSizer);
	boxSizer->Add(m_findButton, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER, 5);

	//results sizer
	resultsSizer->Add(m_resultsList, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);


	mainSizer->Add(boxSizer, 0, wxALL, 7);
	mainSizer->Add(resultsSizer, 0, wxGROW | wxBOTTOM | wxRIGHT | wxLEFT, 7);

	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);
}
//--------------------------------------------------------------------------------------
void GRFindPlayerWindow::OnSearch(wxCommandEvent &event)
{
	(void)event;

	m_resultsList->DeleteAllItems();
	if(m_accountValue->GetValue().Len() > 0)
	{
		searchByAccount();
	}
	else if(m_nickValue->GetValue().Len() > 0)
	{
		searchByNick();
	}
	else if(m_emailValue->GetValue().Len() > 0)
	{
		searchByEmail();
	}
	else if(m_userIDValue->GetValue().Len() > 0)
	{
		searchByID();
	}
}
//---------------------------------------------------------------------------------------
void GRFindPlayerWindow::searchByID()
{
	wxUint8 *payload = NULL;
	wxUint32 len = 0;
	wxUint32 type;
	wxUint32 userID;


	//add up lengths
	len += sizeof(wxUint32)*2;

	//allocate memory
	payload = new wxUint8[len];

	//search type - account name
	type = SEARCH_BY_ID;
	type = wxUINT32_SWAP_ON_LE(type);

	//user id
	m_userIDValue->GetValue().ToULong((unsigned long*)&userID);
	userID = wxUINT32_SWAP_ON_LE(userID);

	//copy data to our payload
	memcpy(payload, &type, sizeof(wxUint32));
	memcpy(payload+sizeof(wxUint32), &userID, sizeof(wxUint32));

	//send it
	GRApplication::getInstance()->getMainConnection()->sendGRPacket(FIND_USER, len, payload);

	if(payload != NULL) delete[] payload;
}
//--------------------------------------------------------------------------------------
void GRFindPlayerWindow::searchByEmail()
{
	wxUint8 *payload = NULL;
	wxUint32 len = 0;
	wxUint32 type;


	//add up lengths
	len += sizeof(wxUint32);
	len += m_emailValue->GetValue().Len() + 1;

	//allocate memory
	payload = new wxUint8[len];

	//search type - account name
	type = SEARCH_BY_EMAIL;
	type = wxUINT32_SWAP_ON_LE(type);

	//copy data to our payload
	memcpy(payload, &type, sizeof(wxUint32));
	strcpy((char*)payload+sizeof(wxUint32), (char*)(const char*)m_emailValue->GetValue().mb_str());

	//send it
	GRApplication::getInstance()->getMainConnection()->sendGRPacket(FIND_USER, len, payload);

	if(payload != NULL) delete[] payload;
}
//----------------------------------------------------------------------------------------
void GRFindPlayerWindow::searchByNick()
{
	wxUint8 *payload = NULL;
	wxUint32 len = 0;
	wxUint32 type;


	//add up lengths
	len += sizeof(wxUint32);
	len += m_nickValue->GetValue().Len() + 1;

	//allocate memory
	payload = new wxUint8[len];

	//search type - account name
	type = SEARCH_BY_NICK;
	type = wxUINT32_SWAP_ON_LE(type);

	//copy data to our payload
	memcpy(payload, &type, sizeof(wxUint32));
	strcpy((char*)payload+sizeof(wxUint32), (char*)(const char*)m_nickValue->GetValue().mb_str());

	//send it
	GRApplication::getInstance()->getMainConnection()->sendGRPacket(FIND_USER, len, payload);

	if(payload != NULL) delete[] payload;
}
//----------------------------------------------------------------------------------------
void GRFindPlayerWindow::searchByAccount()
{
	wxUint8 *payload = NULL;
	wxUint32 len = 0;
	wxUint32 type;


	//add up lengths
	len += sizeof(wxUint32);
	len += m_accountValue->GetValue().Len() + 1;

	//allocate memory
	payload = new wxUint8[len];

	//search type - account name
	type = SEARCH_BY_ACCOUNT;
	type = wxUINT32_SWAP_ON_LE(type);

	//copy data to our payload
	memcpy(payload, &type, sizeof(wxUint32));
	strcpy((char*)payload+sizeof(wxUint32), (char*)(const char*)m_accountValue->GetValue().mb_str());

	//send it
	GRApplication::getInstance()->getMainConnection()->sendGRPacket(FIND_USER, len, payload);

	if(payload != NULL) delete[] payload;
}
//----------------------------------------------------------------------------------------
void GRFindPlayerWindow::findUserResults(GR_PACKET *Packet)
{
	wxUint32 searchType, userID, resultCount, x;
	wxUint8 *ptr;
	int index;
	wxListItem item, item1;
	wxFont font;
	wxString tempStr, nickname, accountName;

	ptr = Packet->payload;
	m_resultsList->DeleteAllItems();
	
	//What search type?
	memcpy(&searchType, ptr, sizeof(wxUint32));
	searchType = wxUINT32_SWAP_ON_LE(searchType);
	ptr += sizeof(wxUint32);

	switch(searchType)
	{
		case SEARCH_BY_ID:
			memcpy(&userID, ptr, sizeof(wxUint32));
			userID = wxUINT32_SWAP_ON_LE(userID);
			ptr += sizeof(wxUint32);
		break;

		case SEARCH_BY_EMAIL:
			tempStr = GRUtil::getInstance()->bufToStr(ptr);
			ptr += tempStr.Len() + 1;
		break;
	
		case SEARCH_BY_NICK:
			tempStr = GRUtil::getInstance()->bufToStr(ptr);
			ptr += tempStr.Len() + 1;
		break;

		case SEARCH_BY_ACCOUNT:
			tempStr = GRUtil::getInstance()->bufToStr(ptr);
			ptr += tempStr.Len() + 1;
		break;
	}

	//result count
	memcpy(&resultCount, ptr, sizeof(wxUint32));
	resultCount = wxUINT32_SWAP_ON_LE(resultCount);
	ptr += sizeof(wxUint32);

	//update label
	resultsSizer->GetStaticBox()->SetLabel(wxT("Search Results: ")+tempStr+wxString::Format(wxT("(%d hits)"), resultCount));

	//font
	font.SetWeight(wxFONTWEIGHT_BOLD);

	for(x = 0; x < resultCount; x++)
	{
		//gr id
		memcpy(&userID, ptr, sizeof(wxUint32));
		userID = wxUINT32_SWAP_ON_LE(userID);
		ptr += sizeof(wxUint32);

		//nickname
		nickname = GRUtil::getInstance()->bufToStr(ptr);
		ptr += nickname.Len() + 1;

		//account name
		accountName = GRUtil::getInstance()->bufToStr(ptr);
		ptr += accountName.Len() + 1;

		if(accountName.Len() > 0) //show id
		{
			index = m_resultsList->InsertItem(m_resultsList->GetItemCount(), accountName);
		}
		else //show premium account name
		{
			index = m_resultsList->InsertItem(m_resultsList->GetItemCount(), wxString::Format(wxT("%d"), userID));
		}
		
		//acount name/id
		item.m_itemId = index;
		item.SetText(nickname);
		if(accountName.Len() > 0)
			font.SetWeight(wxFONTWEIGHT_BOLD);
		else
			font.SetWeight(wxFONTWEIGHT_NORMAL);
		item.SetFont(font);
		m_resultsList->SetItem(item);


		//nickname
		font.SetWeight(wxNORMAL);

		m_resultsList->SetItem(index, 1, nickname);
		m_resultsList->SetItemData(index, (long)userID);
	}
}
//------------------------------------------------------------------------------------
void GRFindPlayerWindow::OnUserDoubleClick(wxListEvent& event)
{
	int index;
	GRUser *user;
	wxUint32 userID;
	
	index = event.GetIndex();
	if(index == -1) return;

	userID = (wxUint32)m_resultsList->GetItemData(index);
	userID = wxUINT32_SWAP_ON_LE(userID);

	GRApplication::getInstance()->getMainConnection()->sendGRPacket(GET_USER_INFO, sizeof(wxUint32), (wxUint8*)&userID);
}
//---------------------------------------------------------------------------------------