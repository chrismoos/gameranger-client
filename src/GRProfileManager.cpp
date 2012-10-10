/*
GameRanger
GRProfileManager.cpp

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

#include "GRProfileManager.h"
#include "GRLogger.h"

/* Profile manager instance */
GRProfileManager *GRProfileManager::_instance = NULL;

GRProfileManager::GRProfileManager()
{

}
/*-------------------------------------------------------------------------*/
GRProfileManager::~GRProfileManager()
{
	for(unsigned int x = 0; x < Profiles.size(); x++) {
		Profiles[x]->Write();
		delete(Profiles[x]);
	}
	Profiles.clear();
	_instance = NULL;
}
/*-------------------------------------------------------------------------*/
GRProfileManager *GRProfileManager::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRProfileManager();
	}
	return _instance;
}
/*-------------------------------------------------------------------------*/
void GRProfileManager::loadProfiles()
{
	wxDir dir;
	wxString filename;
	GRProfile *profile;

	if(!dir.Exists(wxGetCwd()+wxT("/profiles"))) {
		GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Unable to load profiles: Directory does not exist(profiles)"));
		return;
	}
		
	dir.Open(wxGetCwd()+wxT("/profiles"));

	if(!dir.IsOpened()) 
	{
		GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Unable to load profiles: Unable to open directory(profiles)"));
		return;
	}

	bool cont = dir.GetFirst(&filename, wxT(""), wxDIR_FILES);
	while(cont)
	{
		profile = new GRProfile();
		profile->Read(wxGetCwd()+wxT("/profiles/")+filename);
		Profiles.push_back(profile);
		cont = dir.GetNext(&filename);
	}

	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Profiles successfully loaded."));
}
/*-------------------------------------------------------------------------*/
vector<GRProfile*> GRProfileManager::getProfiles()
{
	return Profiles;
}
/*-------------------------------------------------------------------------*/
void GRProfileManager::addProfile(GRProfile *profile)
{
	Profiles.push_back(profile);
}
/*-------------------------------------------------------------------------*/



