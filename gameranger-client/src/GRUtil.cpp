/*
GameRanger
GRUtil.cpp

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

#include "GRUtil.h"
#include "GRPlugin.h"

/* GR Utility instance */
GRUtil *GRUtil::_instance = NULL;

GRUtil::GRUtil()
{

}
/*----------------------------------------------------------------------------*/
GRUtil *GRUtil::getInstance()
{
	if(_instance == NULL) {
		_instance = new GRUtil();
	}
	return _instance;
}
/*----------------------------------------------------------------------------*/
wxString GRUtil::bufToStr(wxUint8 *text)
{
	wxUint32 x;
	wxString szNew;

	for(x = 0;;x++) {
		if(text[x] == 0) break;
		szNew += wxString::Format(wxT("%c"), text[x]);
	}

	return szNew;
}
/*----------------------------------------------------------------------------*/
wxUint8 *GRUtil::makeGameList(vector <GRPlugin*> list)
{
	int *plgList,x,y,z;
	wxUint8 *buf = NULL;
	wxUint8 byteCount;
	wxUint8 temp;
	int listSize;
	wxUint8 t1;
	int offset = 0;

	if(list.size() == 0)
	{
		buf = new wxUint8[1];
		byteCount = 0;
		memcpy(buf, &byteCount, 1);
		return buf;
	}

	plgList = new int[list.size()];

	//fill plugin list array
	for(x = 0; x < list.size(); x++)
	{
		plgList[x] = list[x]->gameCode;
	}

	//sort
	sort(plgList, plgList+list.size());

	byteCount = (plgList[list.size()-1]/8) + 1;
	buf = new wxUint8[byteCount+1];

	memcpy(buf, &byteCount, 1);
	listSize = list.size();

	for(x = 0; x < byteCount; x++)
	{
		temp = 0;
		for(y = 0; y < 8; y++)
		{
			for(z = 0; z < listSize; z++)
			{
				if(plgList[z] > (x*8) && plgList[z] <= ((x+1)*8))
				{
				t1 = plgList[z] - (x*8);
				if(t1 == 1)
					temp |= 1;
				if(t1 == 2)
					temp |= 2;
				if(t1 == 3)
					temp |= 4;
				if(t1 == 4)
					temp |= 8;
				if(t1 == 5)
					temp |= 16;
				if(t1 == 6)
					temp |= 32;
				if(t1 == 7)
					temp |= 64;
				if(t1 == 8)
					temp |= 128;
				}
			}
			
		}
		memcpy(buf+(x+1), &temp, 1);
	}
	return buf;
}
//--------------------------------------------------------------------------------------
