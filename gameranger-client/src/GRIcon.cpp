/*
GameRanger
GRIcon.cpp

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

#include "GRIcon.h"
#include "memdebug.h"


GRIcon::GRIcon()
{
	iconID = 0;
	imageIndex = 0; //default icon
	image = NULL;
}
//------------------------------------------------------------------------
GRIcon::~GRIcon()
{
	if(image != NULL) delete(image);
}
//--------------------------------------------------------------------------
void GRIcon::SetIconData(wxUint8 *data)
{
	memcpy(iconData, data, 256);
	image = iconDataToImage(data);
}
//-------------------------------------------------------------------------
void GRIcon::SetIconID(wxUint32 iconID)
{
	this->iconID = iconID;
}
//-------------------------------------------------------------------------
wxImage *GRIcon::iconDataToImage(wxUint8 *data)
{
	wxUint32 x, y, index;
	wxUint8 *ptr;
	wxImage *img = new wxImage(16, 16);
	ptr = data;

	if(colorTable == NULL) return NULL;

	for(y = 0; y < 16; y++) 
	{
		for(x = 0; x < 16; x++, ptr++) 
		{
			index = (*ptr)*3;
			img->SetRGB(x, y, colorTable[index], colorTable[index+1], colorTable[index+2]);
		}
	}
	return img;
}
//-------------------------------------------------------------------------
