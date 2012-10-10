/*
GameRanger
GRConnectStatusWindow.cpp

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


#include "GRConnectStatusWindow.h"
#include "memdebug.h"

/* status instance */
GRConnectStatusWindow *GRConnectStatusWindow::_instance = NULL;

GRConnectStatusWindow::GRConnectStatusWindow(const wxFrame *parent, const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame*)parent, -1, title, pos, size, wxCAPTION)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);


	statusIndicator = new wxGauge(this, -1, 5, wxDefaultPosition, 
		wxSize(300, 30));

	topSizer->Add(statusIndicator, 0, wxALL | wxALIGN_CENTER, 15);


	SetSizer(topSizer);
	topSizer->Fit(this);
	topSizer->SetSizeHints(this);

	CentreOnScreen();
}
/*------------------------------------------------------------------------------------------*/
GRConnectStatusWindow::~GRConnectStatusWindow()
{
	
}
/*------------------------------------------------------------------------------------------*/
GRConnectStatusWindow *GRConnectStatusWindow::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRConnectStatusWindow(NULL, wxT("Connecting to server..."), wxDefaultPosition, wxDefaultSize);
	}
	return _instance;
}
/*------------------------------------------------------------------------------------------*/
GRConnectStatusWindow *GRConnectStatusWindow::getInstance(wxFrame *parent)
{
	if(_instance == NULL) {
		_instance = new GRConnectStatusWindow(parent, wxT("Connecting to server..."), wxDefaultPosition, wxDefaultSize);
	}
	return _instance;
}
/*------------------------------------------------------------------------------------------*/
