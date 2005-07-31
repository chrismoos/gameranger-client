/*
GameRanger
GRProfile.h

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

#ifndef GRPROFILE_H
#define GRPROFILE_H

#include "GRBaseDefs.h"

class GRProfile
{
public:
	GRProfile();
	~GRProfile();

	
	void Write();
	void Read(wxString filename);
	void makeMac();

	//Profile Options
	wxUint32 grID;
	wxString email;
	wxString nickname;
	wxString realname;
	wxUint8 macAddress[6];
	wxUint8 *gamesList;
	wxString password;
	bool savePass;

	/* Set games list */
	void setGamesList(wxUint8 *list);

	wxUint32 comboIndex;

	wxString bufToStr(wxUint8 *text);
};

#endif

