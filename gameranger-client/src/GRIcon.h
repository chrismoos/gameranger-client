/*
GameRanger
GRIcon.h

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

#ifndef GRICON_H
#define GRICON_H

#include "GRBaseDefs.h"


class GRIcon 
{
public:
	GRIcon();
	~GRIcon();

	wxUint32 iconID;
	wxUint32 imageIndex;
	wxUint8 iconData[256];
	wxUint8 *colorTable;

	//Set Icon Data and ID
	void SetIconData(wxUint8 *data);
	void SetIconID(wxUint32 iconID);

	//Create Image
	wxImage *iconDataToImage(wxUint8 *data);
	wxImage *image;
};

#endif
