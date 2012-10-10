/*
GRPlugin.h

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

#ifndef GRPLUGIN_H
#define GRPLUGIN_H

#include "GRBaseDefs.h"

using namespace std;

class GRPlugin
{
public:
	GRPlugin();
	~GRPlugin();

	//Load from file
	void LoadFromFile(wxString filename);

	//Plugin Info
	wxUint32 gameCode;
	wxString gameName;
	wxImage *image;

	//Unknown option needed for login packet plugin list
	wxUint8 unknownOption[4];
	
	//color table
	wxUint8 *colorTable;

	//Icon
	wxImage *iconDataToImage(wxUint8 *data);
	wxInt32 imageIndex;


    
};

struct sortPluginByName
{
    public: bool operator() (GRPlugin *plug1, GRPlugin *plug2)
    {
		int first, second;
		first = plug1->gameName.GetChar(0);
		second = plug2->gameName.GetChar(0);
        if(first >= 97) first -= 32;
		if(second >= 97) second -= 32;
		return first < second;
    }
};


#endif
