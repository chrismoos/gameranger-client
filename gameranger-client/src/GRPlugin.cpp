/*
GameRanger
GRPlugin.cpp

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

#include "GRPlugin.h"
#include "memdebug.h"

GRPlugin::GRPlugin()
{
	gameName = wxT("Default Game Name");
	gameCode = 0xffffffff;
	image = NULL;
	colorTable = NULL;
	imageIndex = -1;
}
//---------------------------------------------------------------------
GRPlugin::~GRPlugin()
{
	if(image != NULL) delete(image);
}
//---------------------------------------------------------------------
bool operator < (const GRPlugin& plug1, const GRPlugin& plug2)
{ 
	return plug1.gameName.GetChar(0) < plug2.gameName.GetChar(0);
}

bool operator == (const GRPlugin& plug1, const GRPlugin &plug2)
{ 
	return plug1.gameName.GetChar(0) == plug2.gameName.GetChar(0);
}
//---------------------------------------------------------------------------
void GRPlugin::LoadFromFile(wxString filename)
{
	wxFile file;
	wxUint8 byte[1];
	char *name;
	wxUint8 *buf, *ptr, *end;
	wxUint8 iconData[256];

	if(!file.Exists(filename)) return;

	file.Open(filename, wxFile::read);

	if(!file.IsOpened()) return;

	//Get Game Code
	file.Seek(0x2D, wxFromStart);
	file.Read(byte, 1);
	gameCode = byte[0];

	//Get Unknown option
	file.Seek(0x32, wxFromStart);
	file.Read(unknownOption, 4);

	//Get Game Name
	file.Seek(0x36, wxFromStart);
	file.Read(byte, 1);
	file.Seek(0x37, wxFromStart);
	name = new char[byte[0]];
	file.Read(name, byte[0]);

	wxString szNew;

	for(wxUint32 x = 0;x < byte[0];x++) {
		szNew += wxString::Format(wxT("%c"), name[x]);
	}

	gameName = szNew;

	//Get Icon Data
	file.Seek(0x01, wxFromStart);
	buf = new wxUint8[file.Length()];
	file.Read(buf, file.Length());

	ptr = buf;
	end = buf+file.Length();

	for(;;ptr++) {
		if(ptr == end-1) break;
		if(*ptr == 's' && ptr[1] == '8') break;

	}
	if(ptr == end-1) {
		file.Close();
		return;
	}

	ptr += 6;

	memcpy(iconData, ptr, 256);

	image = iconDataToImage(iconData);

	file.Close();

	delete[] buf;
	delete[] name;
}
//-------------------------------------------------------------------
wxImage *GRPlugin::iconDataToImage(wxUint8 *data)
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
//--------------------------------------------------------------------

