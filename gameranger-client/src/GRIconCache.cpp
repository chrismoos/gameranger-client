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
#include "GRLogger.h"
#include "GRApplication.h"
#include "memdebug.h"

/* Icon cache instance */
GRIconCache *GRIconCache::_instance = NULL;

DEFINE_EVENT_TYPE(GRLOADCACHEEVENT)

GRIconCache::GRIconCache()
{
	imgList = NULL;

	if(wxFile::Exists(wxT("images/defaulticon.xpm"))) {
		wxString path = wxGetCwd();
		wxBitmap pic3(path+wxT("/images/defaulticon.xpm"), wxBITMAP_TYPE_XPM);
		GRIcon *defaultIcon = new GRIcon();
		defaultIcon->SetIconID(0);
		defaultIcon->imageIndex = 0;
		defaultIcon->image = new wxImage(pic3.ConvertToImage());
		Icons.push_back(defaultIcon);
	}
}
//-----------------------------------------------------------------------
GRIconCache::~GRIconCache()
{
	int x;

	/* Write cache */
	SaveCache(wxT("Icon_Cache.bin"));

	for(x = 0; x < Icons.size(); x++) 
	{
		delete(Icons[x]);
	}
	Icons.clear();
}
//-----------------------------------------------------------------------
GRIconCache *GRIconCache::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRIconCache();
	}
	return _instance;
}
/*-----------------------------------------------------------------------*/
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

	if(Icons.size() == 0) return;

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
void GRIconCache::startLoadCache()
{
	GRIconCacheLoadThread *thread = new GRIconCacheLoadThread();
	thread->Run();	
}
/*-----------------------------------------------------------------------*/
void GRIconCache::LoadCache(wxString filename)
{
	
}
//-------------------------------------------------------------------------



GRIconCacheLoadThread::GRIconCacheLoadThread()
{
	Create();
}
/*-----------------------------------------------------------------------------*/
void *GRIconCacheLoadThread::Entry()
{
	wxUint32 x, count, iconID;
	wxUint8 *buf, *ptr;
	wxUint32 len;
	wxFile file;
	GRIcon *icon;
	wxString filename = wxString(wxT("Icon_Cache.bin"));
	wxCommandEvent evt(GRLOADCACHEEVENT, LOADCACHEDONEID);

	GRIconCache *iconCache = GRIconCache::getInstance();

	//fatal error
	if(iconCache->colorTable == NULL) return 0;  

	if(!file.Exists(filename)) {
		::wxPostEvent(GRApplication::getInstance()->getMainWindow(), evt);
		//GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Icon cache load failed. Creating new icon cache."));
		return 0;
	}

	file.Open(filename, wxFile::read);

	if(!file.IsOpened()) {
		//GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Icon cache load failed. Creating new icon cache."));
		return 0;
	}

	len = file.Length();
	buf = new wxUint8[len];
	if(buf == NULL) return 0;

	file.Read(buf, len);
	ptr = buf;

	if(len < sizeof(wxUint32)) 
	{
		file.Close();
		return 0;
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
		icon->SetIconID(iconID);
		icon->SetIconData(ptr);
		icon->imageIndex = -1;
		iconCache->Icons.push_back(icon);

		ptr += 256;
	}

	file.Close();
	delete[] buf;

	
	::wxPostEvent(GRApplication::getInstance()->getMainWindow(), evt);

	return 0;
}
/*-----------------------------------------------------------------------------*/
