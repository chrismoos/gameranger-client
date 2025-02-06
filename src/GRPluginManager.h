/*
GameRanger
GRPluginManager.h

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

#ifndef GRPLUGINMANAGER_H
#define GRPLUGINMANAGER_H

#include "GRBaseDefs.h"
#include "GRPlugin.h"

DECLARE_EVENT_TYPE(GRPLUGINLOADEVENT, -1)

class GRPluginManager {
public:
	static GRPluginManager *getInstance();

	/* Accessors */
	static wxString getPluginDirectory();

	/* Load Plugins */
	void loadPlugins();
	void addDefaultPlugins();

	/* Load Color Profile */
	void loadColorProfile();

	/* Get Color Table */
	wxUint8 *getColorTable();

	/* Add Plugin */
	void addPlugin(GRPlugin *plugin);

	/* Find Plugin */
	GRPlugin *findPluginByCode(wxUint32 gameCode);

	/* Get plugin count */
	int getPluginCount();

	/* Get plugins */
	vector <GRPlugin*> *getPlugins();
	GRPlugin *getPluginAt(int x);

	~GRPluginManager();
protected:
	GRPluginManager();

private:
	static GRPluginManager *_instance;
	static wxString pluginDirectory;

	/* Plugins */
	vector<GRPlugin*> Plugins;

	/* Color Profile */
	wxUint8 *colorTable;
};

class GRLoadPluginsThread : public wxThread {
public:
	GRLoadPluginsThread();

	void *Entry();
};

enum {
	GRPLUGINLOADDONEID = wxID_HIGHEST+10
};

#endif
