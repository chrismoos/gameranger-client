/*
GameRanger
GRPluginManager.cpp

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

#include "GRPluginManager.h"
#include "GRApplication.h"
#include "GRLogger.h"

/* GRPluginManager instance */
GRPluginManager *GRPluginManager::_instance = NULL;

/* Plugin directory */
wxString GRPluginManager::pluginDirectory = wxString(wxT("plugins"));

DEFINE_EVENT_TYPE(GRPLUGINLOADEVENT)

GRPluginManager::GRPluginManager()
{
	colorTable = NULL;
}
/*------------------------------------------------------------------------------*/
GRPluginManager::~GRPluginManager()
{
	for(size_t x = 0; x < Plugins.size(); x++) {
		delete(Plugins[x]);
	}
	Plugins.clear();
	if(colorTable != NULL) delete[] colorTable;
	_instance = NULL;
}
/*------------------------------------------------------------------------------*/
wxString GRPluginManager::getPluginDirectory()
{
	return pluginDirectory;
}
/*------------------------------------------------------------------------------*/
GRPluginManager *GRPluginManager::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRPluginManager();
	}
	return _instance;
}
/*------------------------------------------------------------------------------*/
void GRPluginManager::addPlugin(GRPlugin *plugin)
{
	Plugins.push_back(plugin);
}
/*------------------------------------------------------------------------------*/
void GRPluginManager::addDefaultPlugins()
{
	GRPlugin *plugin;

	/* Chat Plugin */
	plugin = new GRPlugin();
	plugin->gameCode = 0;
	plugin->gameName = wxT("Chat");
	Plugins.push_back(plugin);
	
	/* Add a plugin that is for all games we don't have */
	plugin = new GRPlugin();
	plugin->gameCode = 0xffffffff;
	plugin->gameName = wxT("Unknown Game");
	Plugins.push_back(plugin);
}
/*------------------------------------------------------------------------------*/
void GRPluginManager::loadPlugins()
{
	/* add default plugins(chat, etc) */
	addDefaultPlugins();

	/* load plugins from directory */
	GRLoadPluginsThread *thread = new GRLoadPluginsThread();
	thread->Run();	
}
/*--------------------------------------------------------------------------------*/
void GRPluginManager::loadColorProfile()
{
	wxFile file;
	wxUint32 size;

	colorTable = NULL;
	if(!file.Exists(wxT("macos.act"))) {
		GRLogger::getInstance()->log(GRLogger::LOG_WARNING, wxT("Color profile could not be found. Icons will not display correctly."));
		return;
	}

	file.Open(wxT("macos.act"), wxFile::read);
	if(!file.IsOpened()) {
		GRLogger::getInstance()->log(GRLogger::LOG_WARNING, wxT("Color profile could not be loaded. Icons will not display correctly."));
		return;
	}

	size = file.Length();

	colorTable = new wxUint8[size];
	if(colorTable == NULL) {
		GRLogger::getInstance()->log(GRLogger::LOG_WARNING, wxT("Color profile could not be initialized. Icons will not display correctly."));
		return;
	}

	file.Read(colorTable, size);
	file.Close();
	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Color profile successfully loaded."));
}
/*--------------------------------------------------------------------------------*/
wxUint8 *GRPluginManager::getColorTable()
{
	return colorTable;
}
/*--------------------------------------------------------------------------------*/
GRPlugin *GRPluginManager::findPluginByCode(wxUint32 gameCode)
{
	wxUint32 x;
	GRPlugin *defaultPlugin = NULL;
	for(x = 0; x < Plugins.size(); x++)
	{
		if(Plugins[x]->gameCode == gameCode) return Plugins[x];
		if(Plugins[x]->gameCode == 0xffffffff) defaultPlugin = Plugins[x];
	}
	if(defaultPlugin == NULL) 
		GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Default plugin does not exist. This can cause problems in the application if a game's plugin isn't found."));
	return defaultPlugin;
}
/*--------------------------------------------------------------------------------*/
int GRPluginManager::getPluginCount()
{
	return Plugins.size();
}
/*--------------------------------------------------------------------------------*/
vector <GRPlugin*> *GRPluginManager::getPlugins()
{
	return &Plugins;
}
/*--------------------------------------------------------------------------------*/
GRPlugin *GRPluginManager::getPluginAt(int x)
{
	return (GRPlugin*)Plugins[x];
}
/*--------------------------------------------------------------------------------*/



GRLoadPluginsThread::GRLoadPluginsThread()
{
	Create();
}
/*--------------------------------------------------------------------------------*/
void *GRLoadPluginsThread::Entry()
{
	wxDir dir;
	wxString filename;
	GRPlugin *plugin;

	/* now let's open plugin directory and add all those */

	if(!dir.Exists(wxGetCwd()+wxT("/")+GRPluginManager::getPluginDirectory())) {
		return 0;
	}

	dir.Open(wxGetCwd()+wxT("/")+GRPluginManager::getPluginDirectory());

	if(!dir.IsOpened()) 
	{
		return 0;
	}

	bool cont = dir.GetFirst(&filename, wxT(""), wxDIR_FILES);
	while(cont)
	{
		plugin = new GRPlugin();
		plugin->colorTable = GRPluginManager::getInstance()->getColorTable();
		plugin->LoadFromFile(wxGetCwd()+wxT("/plugins/")+filename);
		GRPluginManager::getInstance()->addPlugin(plugin);
		cont = dir.GetNext(&filename);
	}

	wxCommandEvent evt(GRPLUGINLOADEVENT, GRPLUGINLOADDONEID);
	::wxPostEvent(GRApplication::getInstance()->getMainWindow(), evt);

	return 0;
}
/*--------------------------------------------------------------------------------*/
