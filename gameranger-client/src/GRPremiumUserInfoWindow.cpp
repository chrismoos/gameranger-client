/*
GameRanger
GRPremiumUserInfoWindow.cpp

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

#include "GRPremiumUserInfoWindow.h"
#include "memdebug.h"

GRPremiumUserInfoWindow::GRPremiumUserInfoWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) parent, -1, title, pos, size, wxSYSTEM_MENU | wxCAPTION)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create controls
	createControls();

	//Center window
	CentreOnScreen();

	//Null it
	m_userInfo = NULL;
	image = NULL;
	m_mainWindow = NULL;

}
//------------------------------------------------------------------------------------
GRPremiumUserInfoWindow::~GRPremiumUserInfoWindow()
{
	if(m_userInfo != NULL) delete(m_userInfo);
	if(image != NULL) delete(image);
	if(m_mainWindow != NULL) m_mainWindow->currentPremiumInfoWindow = NULL;
}
//-------------------------------------------------------------------------------------
void GRPremiumUserInfoWindow::SetInfo(GRPremiumUserInfo *userInfo)
{
	time_t rawtime;
	struct tm * timeinfo;
	wxFont *font = new wxFont(8, wxDEFAULT, wxNORMAL, wxBOLD);
	m_userInfo = userInfo;
	wxIcon userIcon;

	accountNameValue->SetLabel(m_userInfo->m_accountName);
	accountNameValue->SetFont(*font);
	nameValue->SetLabel(m_userInfo->m_realName);

	//signed up
	rawtime = m_userInfo->m_memberSince;
	timeinfo = localtime (&rawtime);
	signedUpValue->SetLabel(ToString(asctime(timeinfo)));

	//web
	webValue->SetLabel(m_userInfo->m_webSite);

	//quote
	quoteValue->SetLabel(m_userInfo->m_quote);

	//fav game
	if(m_userInfo->m_favGame != NULL)
	{
		favGameValue->SetLabel(m_userInfo->m_favGame->gameName);
	}

	if(m_userInfo->m_email == wxT(""))
	{
		emailValue->SetLabel(wxT("(Withheld)"));
	}
	else 
	{
		emailValue->SetLabel(m_userInfo->m_email);
	}

	//logged in since
	rawtime = m_userInfo->m_lastLogin;
	timeinfo = localtime (&rawtime);
	loggedInValue->SetLabel(ToString(asctime(timeinfo)));

	//last login/currently logged in?
	if(m_userInfo->m_lastLogout == 0) //logged in
	{
		loggedInText->SetLabel(wxT("Logged in:"));
		loggedOutText->SetLabel(wxT(""));
		loggedOutValue->SetLabel(wxT(""));
	}
	else 
	{
		rawtime = m_userInfo->m_lastLogout;
		timeinfo = localtime (&rawtime);
		loggedOutValue->SetLabel(ToString(asctime(timeinfo)));
	}

	if(m_userInfo->m_lobby == NULL)
	{
		roomValue->SetLabel(wxT("Offline"));
	}
	else
	{
		roomValue->SetLabel(m_userInfo->m_lobby->lobbyName);
	}
	if(m_userInfo->m_icon != NULL)
	{
		if(m_userInfo->m_icon->image != NULL) 
		{
			userIcon.CopyFromBitmap(wxBitmap(m_userInfo->m_icon->image));
			SetIcon(userIcon);
		}
	}


	//Activate Sizer
	SetAutoLayout(true);
	delete(font);
}
//------------------------------------------------------------------------------------
void GRPremiumUserInfoWindow::createControls()
{
	wxFlexGridSizer *grid = new wxFlexGridSizer(9, 2, 5, 5);
	wxFlexGridSizer *grid1 = new wxFlexGridSizer(2, 2, 5, 5);
	wxFont *font = new wxFont(8, wxDEFAULT, wxNORMAL, wxBOLD);
	wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer = new wxBoxSizer(wxVERTICAL);

	//captions
	wxStaticText *premiumText = new wxStaticText(this, -1, wxT("Premium Member"));
	premiumText->SetFont(*font);
	wxStaticText *blankText = new wxStaticText(this, -1, wxT(""));
	wxStaticText *accountNameText = new wxStaticText(this, -1, wxT("Account:"));
	accountNameText->SetFont(*font);
	wxStaticText *signedUpText = new wxStaticText(this, -1, wxT("Signed up:"));
	signedUpText->SetFont(*font);
	wxStaticText *nameText = new wxStaticText(this, -1, wxT("Real name:"));
	nameText->SetFont(*font);
	wxStaticText *emailText = new wxStaticText(this, -1, wxT("E-mail:"));
	emailText->SetFont(*font);
	wxStaticText *webText = new wxStaticText(this, -1, wxT("Web:"));
	webText->SetFont(*font);
	wxStaticText *favGameText = new wxStaticText(this, -1, wxT("Fave Game:"));
	favGameText->SetFont(*font);
	wxStaticText *quoteText = new wxStaticText(this, -1, wxT("Quote/Notes:"));
	quoteText->SetFont(*font);
	loggedInText = new wxStaticText(this, -1, wxT("Logged in:"));
	loggedInText->SetFont(*font);
	loggedOutText = new wxStaticText(this, -1, wxT("Logged out:"));
	loggedOutText->SetFont(*font);
	wxStaticText *locationText = new wxStaticText(this, -1, wxT("Location:"));
	locationText->SetFont(*font);
	wxStaticText *roomText = new wxStaticText(this, -1, wxT("Room:"));
	roomText->SetFont(*font);

	//values
	nameValue = new wxStaticText(this, -1, wxT("name"), wxDefaultPosition, wxSize(200, 15));
	webValue = new wxStaticText(this, -1, wxT("web"), wxDefaultPosition, wxSize(200, 15));
	quoteValue = new wxStaticText(this, -1, wxT("quote"), wxDefaultPosition, wxSize(200, 30));
	favGameValue = new wxStaticText(this, -1, wxT("fav game"), wxDefaultPosition, wxSize(200, 15));
	emailValue = new wxStaticText(this, -1, wxT("email"), wxDefaultPosition, wxSize(200, 15));
	loggedInValue = new wxStaticText(this, -1, wxT("logged in"), wxDefaultPosition, wxSize(200, 15));
	loggedOutValue = new wxStaticText(this, -1, wxT("logged out"), wxDefaultPosition, wxSize(200, 15));
	roomValue = new wxStaticText(this, -1, wxT("room"), wxDefaultPosition, wxSize(200, 15));
	accountNameValue = new wxStaticText(this, -1, wxT("account name"), wxDefaultPosition, wxSize(140, 15));
	signedUpValue = new wxStaticText(this, -1, wxT("date"), wxDefaultPosition, wxSize(140, 15));

	//map
	image = new wxImage();
	wxImage *image1 = new wxImage();
	wxInitAllImageHandlers();
	image->LoadFile(wxT("images/map.jpg"), wxBITMAP_TYPE_JPEG);
	image1->LoadFile(wxT("images/blank.jpg"), wxBITMAP_TYPE_JPEG);
	wxStaticBitmap *bmp = new wxStaticBitmap(this, -1, wxBitmap(image));
	bmp1 = new wxStaticBitmap(this, -1, wxBitmap(image1), wxDefaultPosition, wxSize(64,64));

	//grid
	grid1->Add(accountNameText);
	grid1->Add(accountNameValue);
	grid1->Add(signedUpText);
	grid1->Add(signedUpValue);

	//right sizer
	rightSizer->Add(premiumText);
	rightSizer->Add(blankText);
	rightSizer->Add(grid1);

	//top sizer
	topSizer->Add(bmp1, 0, wxRIGHT, 10);
	topSizer->Add(rightSizer);

	grid->Add(nameText);
	grid->Add(nameValue);
	grid->Add(emailText);
	grid->Add(emailValue);
	grid->Add(webText);
	grid->Add(webValue);
	grid->Add(favGameText);
	grid->Add(favGameValue);
	grid->Add(quoteText);
	grid->Add(quoteValue);
	grid->Add(loggedInText);
	grid->Add(loggedInValue);
	grid->Add(loggedOutText);
	grid->Add(loggedOutValue);
	grid->Add(locationText);
	grid->Add(bmp);
	grid->Add(roomText);
	grid->Add(roomValue);

	mainSizer->Add(topSizer, 0, wxTOP | wxLEFT | wxRIGHT, 10);
	mainSizer->Add(grid, 0, wxALL | wxALIGN_CENTER, 10);


	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	delete(font);
	delete(image1);
}
//------------------------------------------------------------------------------------
wxString GRPremiumUserInfoWindow::ToString(char *text)
{
	wxUint32 x;
	wxString szNew;

	for(x = 0;;x++) {
		if(text[x] == 0) break;
		szNew += wxString::Format(wxT("%c"), text[x]);
	}

	return szNew;
}
//---------------------------------------------------------------------------------
void GRPremiumUserInfoWindow::SetImage(wxImage *img)
{
	bmp1->SetBitmap(wxBitmap(img));
}
//------------------------------------------------------------------------------------
