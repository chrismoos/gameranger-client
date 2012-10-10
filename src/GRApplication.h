/*
GameRanger
GRApplication.h

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

#ifndef GRAPPLICATION_H
#define GRAPPLICATION_H

#include "GRBaseDefs.h"
#include "GRMainWindow.h"
#include "GRPluginManager.h"
#include "GRIconCache.h"
#include "GRLogger.h"
#include "GRProfileManager.h"
#include "GRProfile.h"
#include "GRMainConnection.h"
#include "GRMainConnection.h"

class wxApp;

using namespace std;

class GRApplication {

public:
	~GRApplication();

	/* Get Application Instance */
	static GRApplication *getInstance();

	/* Start Application */
	void Start();

	/* Accessors */
	GRMainWindow *getMainWindow();
	GRPluginManager *getPluginManager();

	/* Setters */
	void setWxMainApp(wxApp *app);

	/* Main Window Control */
	void createMainWindow();
	void showMainWindow();
	void hideMainWindow();
	bool isMainWindowClosed();
	void mainWindowHasClosed();

	/* Plugin Manager */
	void startPluginManager();

	/* Profile Manager */
	void startProfileManager();

	/* Icon cache */
	void initIconCache();

	/* Login */
	void Login(GRProfile *profile);
	void Login(wxString email, wxString password, bool savePass);

	/* Main Connection */
	void closeMainConnection();
	GRMainConnection *getMainConnection();

	/* Get Main Window mutex */
	static wxMutex mainWindowMutex;

protected:
	GRApplication();

private:
	static GRApplication *_instance;

	/* wxApp */
	wxApp *wxMainApp;

	/* Main Window */
    GRMainWindow *mainWindow;

	/* GRLogger */
	GRLogger *logger;

	/* Plugin Manager */
	GRPluginManager *pluginManager;

	/* Profile Manager */
	GRProfileManager *profileManager;

	/* GR Main Connection */
	GRMainConnection *mainConn;

};

#endif
