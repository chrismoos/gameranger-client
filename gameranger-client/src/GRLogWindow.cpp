/*
GameRanger
GRLogWindow.cpp

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

#include "GRLogWindow.h"
#include "GRMainWindow.h"
#include "memdebug.h"

GRLogWindow::GRLogWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *)parent, -1, title, pos, size)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	textLog = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize(500, 200),
		wxTE_READONLY | wxTE_NOHIDESEL | wxTE_MULTILINE | wxTE_RICH2);

	mainSizer->Add(textLog, 1, wxGROW);

	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);
	mainWindow = NULL;
}
//-------------------------------------------------------------------------------
GRLogWindow::~GRLogWindow()
{
}
//-------------------------------------------------------------------------------
void GRLogWindow::logText(wxString str)
{
	wxString temp;
	
	temp = str + wxT("\n");

#ifdef SHOW_LOG_WINDOW
	textLog->AppendText(temp);
#endif
}
//------------------------------------------------------------------------------
void GRLogWindow::logData(wxUint8 *buf, wxUint32 length)
{
	wxString temp;
	wxUint32 x;
	temp = wxT("Data: ");

	for(x = 0; x < length; x++) 
	{
		temp += wxString::Format(wxT("%02X "), buf[x]);
	}

	temp += wxT("\n\n");
#ifdef SHOW_LOG_WINDOW
	textLog->AppendText(temp);
#endif
}
//------------------------------------------------------------------------------

