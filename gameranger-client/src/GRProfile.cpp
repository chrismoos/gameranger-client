/*
GameRanger
GRProfile.cpp

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

#include "GRProfile.h"
#include "memdebug.h"

GRProfile::GRProfile()
{
	grID = 0;
	nickname = wxT("nick");
	email = wxT("email");
	realname = wxT("name");
	password = wxT("");
	makeMac();
	gamesList = new wxUint8[1];
	gamesList[0] = 0;
}
//------------------------------------------------------------------
GRProfile::~GRProfile()
{
	if(gamesList != NULL) delete[] gamesList;
}
//------------------------------------------------------------------------
void GRProfile::Write()
{
	wxFile file;
	wxUint32 size;
	wxUint8 *buf, *ptr;

	size = sizeof(wxUint32) + 6;
	size += nickname.Len() + 1;
	size += realname.Len() + 1;
	size += email.Len() + 1;
	size += *(gamesList)+1;
	size += password.Len() + 1;


	buf = new wxUint8[size];
	ptr = buf;

	//user id
	grID = htonl(grID);
	memcpy(ptr, &grID, sizeof(wxUint32));
	ptr += sizeof(wxUint32);
	grID = ntohl(grID);

	//mac address
	memcpy(ptr, macAddress, 6);
	ptr += 6;

	//email
	strcpy((char*)ptr, (const char*)email.mb_str());
	ptr += email.Len() + 1;

	//nickname
	strcpy((char*)ptr, (const char*)nickname.mb_str());
	ptr += nickname.Len() + 1;

	//real name
	strcpy((char*)ptr, (const char*)realname.mb_str());
	ptr += realname.Len() + 1;

	//games list
	memcpy(ptr, gamesList, *(gamesList)+1);
	ptr += *(gamesList)+1;

	//password
	strcpy((char*)ptr, (const char*)password.mb_str());
	ptr += password.Len() + 1;

	file.Open(wxT("profiles/")+nickname+wxT(".bin"), wxFile::write);

	file.Write(buf, size);
	file.Close();

	delete[] buf;
	
}
//-----------------------------------------------------------------------
void GRProfile::Read(wxString filename)
{
	wxFile file;
	wxUint8 *buf, *ptr;

	if(!file.Exists(filename)) return;

	file.Open(filename, wxFile::read);

	if(!file.IsOpened()) return;

	buf = new wxUint8[file.Length()+1];
	file.Read(buf, file.Length());
	buf[file.Length()] = 0xFF;
	ptr = buf;
	//Parse 
	

	//gr id
	memcpy(&grID, ptr, sizeof(wxUint32));
	ptr += sizeof(wxUint32);
	grID = ntohl(grID);

	//mac
	memcpy(macAddress, ptr, 6);
	ptr += 6;

	//email
	email = bufToStr(ptr);
	ptr += email.Len() + 1;

	//nickname
	nickname = bufToStr(ptr);
	ptr += nickname.Len() + 1;

	//real name
	realname = bufToStr(ptr);
	ptr += realname.Len() + 1;

	//games list
	if(*ptr != 0xFF)
	{
		if(gamesList != NULL) delete[] gamesList;
		gamesList = new wxUint8[*(ptr)+1];
		memcpy(gamesList, ptr, *(ptr)+1);
		ptr += *(ptr)+1;
	}

	//password
	if(*ptr != 0xFF) 
	{
		password = bufToStr(ptr);
	}
	

	file.Close();

	delete[] buf;
}
//---------------------------------------------------------------------
wxString GRProfile::bufToStr(wxUint8 *text)
{
	wxUint32 x;
	wxString szNew;

	for(x = 0;;x++) {
		if(text[x] == 0) break;
		szNew += wxString::Format(wxT("%c"), text[x]);
	}

	return szNew;
}
//-----------------------------------------------------------------------
void GRProfile::makeMac()
{
	wxUint8 temp[6];
	wxUint32 x;
	temp[0] = 0;
	temp[1] = 3;
	srand(time(NULL));
	for(x = 2; x < 6; x++) temp[x] = rand();
	memcpy(macAddress, temp, 6);
}
//----------------------------------------------------------------------


