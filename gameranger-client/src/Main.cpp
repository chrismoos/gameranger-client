/*
GameRanger
Main.cpp

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

#include "Main.h"
#include "GRBaseDefs.h"
#include <wx/dir.h>
#include "memdebug.h"

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
	m_mainWindow = new GRMainWindow(wxT("GameRanger"), wxDefaultPosition, wxDefaultSize);
	m_mainWindow->Show(true);
	SetTopWindow(m_mainWindow);
/*	if(FirstRun()) 
	{
		//Run reg wizard
		m_regWindow = new GRRegWindow(wxT("reg"), wxDefaultPosition, wxSize(0,0));
		m_regWindow->Show(false);
		SetTopWindow(m_regWindow);

		
	}
	else 
	{
		//Create Login Window
		m_loginWindow = new GRLoginWindow(wxT("Login to GameRanger"), wxDefaultPosition,
			wxSize(200, 200));
		m_loginWindow->Show(true);
		SetTopWindow(m_loginWindow);
	}*/
#ifdef WIN32
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
#endif

	return TRUE;
}
//--------------------------------------------------------------------------------
bool MainApp::FirstRun()
{
	wxDir dir;
	wxString filename;

	if(!dir.Exists(wxGetCwd()+wxT("/profiles"))) return true;

	dir.Open(wxGetCwd()+wxT("/profiles"));

	if(!dir.IsOpened()) 
	{
		return true;
	}

	bool cont = dir.GetFirst(&filename, wxT(""), wxDIR_FILES);
	while(cont)
	{
		return false;
	}	
	return true;
}
//--------------------------------------------------------------------------------
