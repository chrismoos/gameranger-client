/*
GameRanger
GRIconCache.cpp

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

#include "GRIconCache.h"
#include "GRIcon.h"
#include "memdebug.h"

GRIconCache::GRIconCache()
{
	GRIcon *defaultIcon = new GRIcon();
	defaultIcon->SetIconID(0);
	defaultIcon->imageIndex = 0;
	Icons.push_back(defaultIcon);
}
//-----------------------------------------------------------------------
GRIconCache::~GRIconCache()
{
	int x;
	for(x = 0; x < Icons.size(); x++) 
	{
	
		delete(Icons[x]);
	}
}
//-----------------------------------------------------------------------
GRIcon *GRIconCache::findIcon(wxUint32 iconID)
{
	wxUint32 x;
	for(x = 0; x < Icons.size(); x++)
	{
		if(Icons[x]->iconID == iconID) return Icons[x];
	}
	return NULL;
}
//------------------------------------------------------------------------
void GRIconCache::AddIcon(GRIcon *icon)
{
	Icons.push_back(icon);
}
//------------------------------------------------------------------------
void GRIconCache::SaveCache(wxString filename)
{
	wxUint32 x;
	wxUint8 *buf, *ptr;
	wxUint32 len;
	wxFile file;
	GRIcon *icon;
	wxUint32 iconCount, id;

	file.Open(filename, wxFile::write);

	len = sizeof(wxUint32) + ((sizeof(wxUint32)+256)*Icons.size()-1);

	buf = new wxUint8[len];
	ptr = buf;

	iconCount = Icons.size();
	iconCount = htonl(iconCount);
	memcpy(ptr, (wxUint8*)&iconCount, sizeof(wxUint32));
	iconCount = ntohl(iconCount);
	ptr += sizeof(wxUint32);

	for(x = 0; x < iconCount-1; x++)
	{
		icon = Icons[x];
		if(icon->iconID == 0) continue;
		id = icon->iconID;
		id = htonl(id);
		memcpy(ptr, (wxUint8*)&id, sizeof(wxUint32));
		ptr += sizeof(wxUint32);

		memcpy(ptr, icon->iconData, 256);
		ptr += 256;
	}

	file.Write(buf, len);
	
	file.Close();
	
	delete[] buf;
}
//-----------------------------------------------------------------------
void GRIconCache::LoadCache(wxString filename)
{
	wxUint32 x, count, iconID;
	wxUint8 *buf, *ptr;
	wxUint32 len;
	wxFile file;
	GRIcon *icon;

	//fatal error
	if(colorTable == NULL) return; 

	if(!file.Exists(filename)) return;

	file.Open(filename, wxFile::read);

	if(!file.IsOpened()) return;

	len = file.Length();
	buf = new wxUint8[len];
	if(buf == NULL) return;

	file.Read(buf, len);
	ptr = buf;

	if(len < sizeof(wxUint32)) 
	{
		file.Close();
		return;
	}

	//# of icons
	memcpy(&count, ptr, sizeof(wxUint32));
	count = ntohl(count);
	ptr += sizeof(wxUint32);
	
	for(x = 0; x < count; x++)
	{
		memcpy(&iconID, ptr, sizeof(wxUint32));
		iconID = ntohl(iconID);
		ptr += sizeof(wxUint32);

		icon = new GRIcon();
		icon->colorTable = colorTable;
		icon->SetIconID(iconID);
		icon->SetIconData(ptr);
		icon->imageIndex = imgList->Add(*icon->image);
		Icons.push_back(icon);

		ptr += 256;
	}

	file.Close();

	delete[] buf;
}
//-------------------------------------------------------------------------
