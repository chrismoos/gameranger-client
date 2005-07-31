/*
GameRanger
GRLogger.cpp

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

#include "GRLogger.h"
#include "GRApplication.h"

/* GR Logger instance */
GRLogger *GRLogger::_instance = NULL;

wxMutex GRLogger::logMutex;

GRLogger::GRLogger()
{
	/* Set up login window */
	logWindow = new GRLogWindow(NULL, wxT("Log Window."), wxDefaultPosition, wxDefaultSize);
	logWindow->Show(true);
}
/*-----------------------------------------------------------------------------*/
GRLogger::~GRLogger()
{
	_instance = NULL;
}
/*-----------------------------------------------------------------------------*/
GRLogger *GRLogger::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRLogger();
	}
	return _instance;
}
/*-----------------------------------------------------------------------------*/
void GRLogger::log(int type, wxString msg)
{
	/* Check if log window still exists..if it doesn't, return */
	if(logWindow == NULL) return;

	switch(type)
	{
		case GRLogger::LOG_INFO:
			logWindow->logText(wxT("INFO: ") + msg);
		break;

		case GRLogger::LOG_WARNING:
			logWindow->logText(wxT("WARNING: ") + msg);
		break;

		case GRLogger::LOG_ERROR:
			logWindow->logText(wxT("ERROR: ") + msg);
		break;
	}
}
/*-------------------------------------------------------------------------------*/
void GRLogger::logData(wxUint8 *buf, wxUint32 length)
{
	wxString temp;
	wxUint32 x;
	temp = wxT("INFO: Data: ");

	for(x = 0; x < length; x++) 
	{
		temp += wxString::Format(wxT("%02X "), buf[x]);
	}

	temp += wxT("\n\n");
	logWindow->logText(temp);
}
/*-------------------------------------------------------------------------------*/
void GRLogger::setWindowDestroyed()
{
	logWindow = NULL;
}
/*-------------------------------------------------------------------------------*/
