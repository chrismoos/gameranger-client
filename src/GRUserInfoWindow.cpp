/*
GameRanger
GRUserInfoWindow.cpp

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

#include "GRUserInfoWindow.h"
#include "memdebug.h"

GRUserInfoWindow::GRUserInfoWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) parent, -1, title, pos, size)
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

}
//------------------------------------------------------------------------------------
GRUserInfoWindow::~GRUserInfoWindow()
{
	if(m_userInfo != NULL) delete(m_userInfo);
	if(image != NULL) delete(image);
}
//-------------------------------------------------------------------------------------
void GRUserInfoWindow::SetInfo(GRUserInfo *userInfo)
{
	time_t rawtime;
	struct tm * timeinfo;
	m_userInfo = userInfo;
	wxIcon userIcon;
	

	accountValue->SetLabel(wxString::Format(wxT("%d"), m_userInfo->m_userID));
	nickValue->SetLabel(m_userInfo->m_nickname);
	nameValue->SetLabel(m_userInfo->m_realName);

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
			userIcon.CopyFromBitmap(wxBitmap(*m_userInfo->m_icon->image));
			SetIcon(userIcon);
			usrbmp->SetBitmap(wxBitmap(*m_userInfo->m_icon->image));
		}
	}


	//Activate Sizer
	SetAutoLayout(true);
}
//------------------------------------------------------------------------------------
void GRUserInfoWindow::createControls()
{
	wxFlexGridSizer *grid = new wxFlexGridSizer(9, 2, 5, 5);
	wxFont *font = new wxFont(8, wxDEFAULT, wxNORMAL, wxBOLD);
	mainSizer = new wxBoxSizer(wxVERTICAL);

	//captions
	wxStaticText *iconText = new wxStaticText(this, -1, wxT("Icon:"));
	iconText->SetFont(*font);
	wxStaticText *accountText = new wxStaticText(this, -1, wxT("Account ID:"));
	accountText->SetFont(*font);
	wxStaticText *nickText = new wxStaticText(this, -1, wxT("Nickname:"));
	nickText->SetFont(*font);
	wxStaticText *nameText = new wxStaticText(this, -1, wxT("Real name:"));
	nameText->SetFont(*font);
	wxStaticText *emailText = new wxStaticText(this, -1, wxT("E-mail:"));
	emailText->SetFont(*font);
	loggedInText = new wxStaticText(this, -1, wxT("Logged in:"));
	loggedInText->SetFont(*font);
	loggedOutText = new wxStaticText(this, -1, wxT("Logged out:"));
	loggedOutText->SetFont(*font);
	wxStaticText *locationText = new wxStaticText(this, -1, wxT("Location:"));
	locationText->SetFont(*font);
	wxStaticText *roomText = new wxStaticText(this, -1, wxT("Room:"));
	roomText->SetFont(*font);

	//values
	accountValue = new wxStaticText(this, -1, wxT("0"), wxDefaultPosition, wxSize(200, 20));
	nickValue = new wxStaticText(this, -1, wxT("nick"), wxDefaultPosition, wxSize(200, 20));
	nameValue = new wxStaticText(this, -1, wxT("name"), wxDefaultPosition, wxSize(200, 15));
	emailValue = new wxStaticText(this, -1, wxT("email"), wxDefaultPosition, wxSize(200, 15));
	loggedInValue = new wxStaticText(this, -1, wxT("logged in"), wxDefaultPosition, wxSize(200, 15));
	loggedOutValue = new wxStaticText(this, -1, wxT("logged out"), wxDefaultPosition, wxSize(200, 15));
	roomValue = new wxStaticText(this, -1, wxT("room"), wxDefaultPosition, wxSize(200, 15));

	//map
	image = new wxImage();
	image->LoadFile(wxT("images/map.jpg"), wxBITMAP_TYPE_JPEG);
	wxStaticBitmap *bmp = new wxStaticBitmap(this, -1, wxBitmap(*image));
	usrbmp = new wxStaticBitmap(this, -1, wxNullBitmap, wxDefaultPosition, wxSize(16,16));

	grid->Add(iconText);
	grid->Add(usrbmp);
	grid->Add(accountText);
	grid->Add(accountValue);
	grid->Add(nickText);
	grid->Add(nickValue);
	grid->Add(nameText);
	grid->Add(nameValue);
	grid->Add(emailText);
	grid->Add(emailValue);
	grid->Add(loggedInText);
	grid->Add(loggedInValue);
	grid->Add(loggedOutText);
	grid->Add(loggedOutValue);
	grid->Add(locationText);
	grid->Add(bmp);
	grid->Add(roomText);
	grid->Add(roomValue);

	mainSizer->Add(grid, 0, wxALL | wxALIGN_CENTER, 10);


	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	delete(font);
}
//------------------------------------------------------------------------------------
wxString GRUserInfoWindow::ToString(char *text)
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