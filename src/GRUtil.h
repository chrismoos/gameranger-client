/*
GameRanger
GRUtil.h

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

#ifndef GRUTIL_H
#define GRUTIL_H

#include "GRBaseDefs.h"

class GRPlugin;

using namespace std;

class GRUtil {
public:
	static GRUtil *getInstance();

	wxString bufToStr(wxUint8 *text);

	/* make games list */
	wxUint8 *makeGameList(vector <GRPlugin*> list);
	
protected:
	GRUtil();

private:
	static GRUtil *_instance;
};

#endif
