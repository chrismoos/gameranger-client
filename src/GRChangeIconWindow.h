/*
GameRanger
GRChangeIconWindow.h

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

#ifndef GRCHANGEICON_WINDOW_H
#define GRCHANGEICON_WINDOW_H

#include "GRBaseDefs.h"
#include "GRMainWindow.h"

class GRChangeIconWindow : public wxFrame
{
public:
	GRChangeIconWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size);
	~GRChangeIconWindow();

	//gui elements
	wxTextCtrl *m_imgLocation;
	wxButton *m_loadButton;
	wxButton *m_changeButton;
	wxImage *m_img;
	wxStaticBitmap *bmp;

	//create controls
	void createControls();

	//main window
	GRMainWindow *m_mainWindow;

	//icon
	int findColorIndex(unsigned char red, unsigned char green, unsigned char blue);
	wxInt32 makeChecksum(wxUint8 *iconData);

	//mac palette
	wxPalette *getMacPalette();
	wxPalette *plt;

	//Events
	void OnLoadIcon(wxCommandEvent &event);
	void OnChangeIcon(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};

enum {
	LOAD_BUTTON_ID = 1000,
	CHANGE_ICON_BUTTON_ID
};

#endif
