/*
GameRanger
GRIconCache.h

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

#ifndef GRICONCACHE_H
#define GRICONCACHE_H

#include "GRBaseDefs.h"

class GRIcon;
using namespace std;

DECLARE_EVENT_TYPE(GRLOADCACHEEVENT, -1)

class GRIconCache
{
public:
	~GRIconCache();

	/* Get Instance */
	static GRIconCache *getInstance();


	vector<GRIcon*> Icons;
	wxUint8 *colorTable;
	wxImageList *imgList;

	/* Find Icon */
	GRIcon *findIcon(wxUint32 iconID);

	/* Add Icon */
	void AddIcon(GRIcon *icon);

	/* Start load cache thread */
	void startLoadCache();

	//Load/Save
	void LoadCache(wxString filename);
	void SaveCache(wxString filename);

protected:
	GRIconCache();

private:
	static GRIconCache *_instance;
};

class GRIconCacheLoadThread : public wxThread {
public:
	GRIconCacheLoadThread();

	void *Entry();
};


enum {
	LOADCACHEDONEID = wxID_HIGHEST + 400
};

#endif
