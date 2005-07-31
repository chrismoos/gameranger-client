/*
GameRanger
GRApplication.cpp

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

#include "GRApplication.h"

/* GR App Instance */
GRApplication *GRApplication::_instance = NULL;

/* Main Window Mutex */
wxMutex GRApplication::mainWindowMutex;

GRApplication::GRApplication()
{
	mainWindow = NULL;
	wxMainApp = NULL;
	profileManager = NULL;
	pluginManager = NULL;
	mainConn = NULL;

	/* Get Logger */
	logger = GRLogger::getInstance();
}
/*---------------------------------------------------------------------------------*/
GRApplication::~GRApplication()
{
	closeMainConnection();
	
	/* Clean up managers */
	delete(GRPluginManager::getInstance());
	delete(GRProfileManager::getInstance());
	delete(GRLogger::getInstance());
	delete(GRIconCache::getInstance());
}
/*---------------------------------------------------------------------------------*/
GRApplication *GRApplication::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRApplication();
	}
	return _instance;
}
/*---------------------------------------------------------------------------------*/
void GRApplication::Start()
{
	/* ---------Application Startup --------------- */

	try {
		/* Create and show our main window */
		createMainWindow();
		showMainWindow();

		/* Start plugin manager */
		startPluginManager();

		/* Start profile manager */
		startProfileManager();

		/* Initialize icon cache */
		initIconCache();
		
		logger->log(GRLogger::LOG_INFO, wxT("Main window successfully created and displayed."));
	}
	catch(wxString errorMsg) {
		logger->log(GRLogger::LOG_ERROR, wxT("Application Startup: ") + errorMsg);
	}

	/* ---------Application Startup Complete --------------- */
}
/*---------------------------------------------------------------------------------*/
GRMainWindow *GRApplication::getMainWindow()
{
	if(mainWindow == NULL) {
		mainWindow = new GRMainWindow(wxT("GameRanger"), wxDefaultPosition, wxDefaultSize);
	}
	return mainWindow;
}
/*---------------------------------------------------------------------------------*/
void GRApplication::createMainWindow()
{
	getMainWindow();
	if(mainWindow == NULL) throw wxString(wxT("Unable to create main window"));
}
/*---------------------------------------------------------------------------------*/
void GRApplication::showMainWindow()
{
	if(mainWindow == NULL) {
		throw wxString("Unable to show window: Main Window does not exist");
	}
	else {
		mainWindow->Show(true);
	}
}
/*---------------------------------------------------------------------------------*/
void GRApplication::hideMainWindow()
{
	if(mainWindow == NULL) {
		throw wxString("Unable to hide window: Main Window does not exist");
	}
	else {
		mainWindow->Show(false);
	}
}
/*---------------------------------------------------------------------------------*/
void GRApplication::setWxMainApp(wxApp *app)
{
	wxMainApp = app;
}
/*---------------------------------------------------------------------------------*/
GRPluginManager *GRApplication::getPluginManager()
{
	return pluginManager;
}
/*---------------------------------------------------------------------------------*/
void GRApplication::startPluginManager()
{
	pluginManager = GRPluginManager::getInstance();
	pluginManager->loadPlugins();
	pluginManager->loadColorProfile();
}
/*---------------------------------------------------------------------------------*/
void GRApplication::startProfileManager()
{
	profileManager = GRProfileManager::getInstance();
	profileManager->loadProfiles();
}
/*---------------------------------------------------------------------------------*/
void GRApplication::initIconCache()
{
	wxInitAllImageHandlers();
	GRIconCache *iconCache = GRIconCache::getInstance();	
	iconCache->colorTable = pluginManager->getColorTable();
	iconCache->startLoadCache();
}
/*---------------------------------------------------------------------------------*/
void GRApplication::Login(GRProfile *profile)
{
	if(mainConn == NULL) mainConn = new GRMainConnection();
	mainConn->setLoginParams(profile);
	
	mainConn->connectToGR();

	/*
	mainConn->testPacket(wxT("data/") + wxString::Format(wxT("%08X.bin"), SERVER_MESSAGE));
	mainConn->testPacket(wxT("data/") + wxString::Format(wxT("%08X.bin"), MY_USER_INFO));
	mainConn->testPacket(wxT("data/") + wxString::Format(wxT("%08X.bin"), PUBLIC_LOBBY_LIST));
	mainConn->testPacket(wxT("data/") + wxString::Format(wxT("%08X.bin"), LOBBY_USER_LIST));
	mainConn->testPacket(wxT("data/") + wxString::Format(wxT("%08X.bin"), GAME_ROOMS_LIST));
	*/
	/*	testPacket(wxT("data/0000000F.bin"));
	testPacket(wxT("data/00000013.bin"));
	testPacket(wxT("data/000000BC.bin"));
	testPacket(wxT("data/00000019.bin"));
	testPacket(wxT("data/0000001A.bin"));
	testPacket(wxT("data/0000000A.bin"));
	testPacket(wxT("data/00000038.bin"));
	testPacket(wxT("data/0000005F.bin"));
	testPacket(wxT("data/0000001F.bin"));*/
}
/*-----------------------------------------------------------------------------------*/
void GRApplication::Login(wxString email, wxString password, bool savePass)
{
	if(mainConn == NULL) mainConn = new GRMainConnection();
	mainConn->setLoginParams(email, password, savePass);
	mainConn->connectToGR();
}
/*-----------------------------------------------------------------------------------*/
void GRApplication::closeMainConnection()
{
	if(mainConn != NULL) {
		delete(mainConn);
		mainConn = NULL;
	}
}
/*-----------------------------------------------------------------------------------*/
bool GRApplication::isMainWindowClosed()
{
	if(mainWindow == NULL) return true;
	return false;
}
/*-----------------------------------------------------------------------------------*/
void GRApplication::mainWindowHasClosed()
{
	mainWindow = NULL;
}
/*-----------------------------------------------------------------------------------*/
GRMainConnection *GRApplication::getMainConnection()
{
	if(mainConn == NULL) {
		mainConn = new GRMainConnection();
	}
	return mainConn;
}
/*-----------------------------------------------------------------------------------*/


