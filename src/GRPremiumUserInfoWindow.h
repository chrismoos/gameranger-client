/*
GameRanger
GRPremiumUserInfoWindow.h

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

#ifndef GRPREMIUMUSERINFO_WINDOW_H
#define GRPREMIUMUSERINFO_WINDOW_H

#include "GRPremiumUserInfo.h"
#include "GRMainWindow.h"

class GRPremiumUserInfoWindow : public wxFrame
{
public:
	GRPremiumUserInfoWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size);
	~GRPremiumUserInfoWindow();

	//GUI Controls
	wxStaticText *iconValue, *nameValue, *emailValue,
		*loggedInValue, *loggedOutValue, *timeValue, *roomValue, *accountNameValue, *signedUpValue,
		*webValue, *favGameValue, *quoteValue;
	wxBoxSizer *mainSizer;
	wxStaticText *loggedOutText, *loggedInText;
	wxStaticBitmap *usrbmp;

	//Map
	wxImage *image;
	wxStaticBitmap *bmp1;
	
	//main window
	GRMainWindow *m_mainWindow;

	//Set premium image
	void SetImage(wxImage *img);

	//Create Controls
	void createControls();

	//Events

	//Misc
	wxString ToString(char *text);

	//User Info
	void SetInfo(GRPremiumUserInfo *userInfo);
	GRPremiumUserInfo *m_userInfo;
	
};

#endif
