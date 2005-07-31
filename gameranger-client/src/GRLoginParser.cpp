/*
GameRanger
GRLoginParser.cpp

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

#include "GRLoginParser.h"
#include "GRApplication.h"
#include "GRConnectStatusWindow.h"
#include "GRLoginWindow.h"
#include "GRLogger.h"
#include "GRUtil.h"

GRLoginParser::GRLoginParser(GRMainConnection *conn)
{
	this->conn = conn;
}
/*--------------------------------------------------------------*/
GRLoginParser::~GRLoginParser()
{

}
/*--------------------------------------------------------------*/
bool GRLoginParser::run(GR_PACKET *Packet)
{
	switch(Packet->header->command)
	{
		case READY_TO_PROCESS:
			conn->loginToGR(Packet);
		break;

		case SERVER_MESSAGE:
			serverMessage(Packet);
		break;

		case BAN_TIME_LEFT:
			banTimeLeft(Packet);
		break;

		case MY_USER_INFO:
			myUserInfo(Packet);
		break;

		default:
			return false;
		break;
	}

	return true;
}
/*--------------------------------------------------------------*/
void GRLoginParser::banTimeLeft(GR_PACKET *Packet)
{
	GR_BAN_TIME_PACKET *banPacket;
	wxUint32 seconds_left, remain;
	wxUint32 days, hours, minutes, seconds;
	char tleft[64];

	banPacket = (GR_BAN_TIME_PACKET*)Packet->payload;

	banPacket->time_left = ntohl(banPacket->time_left);

	

	seconds_left = banPacket->time_left;

	days = seconds_left/86400;
	remain = seconds_left % 86400;
	hours = remain/3600;
	remain = seconds_left % 3600;
	minutes = remain/60;
	remain = seconds_left % 60;
	seconds = remain;

	sprintf(tleft, "%d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
	wxString detail = GRUtil::getInstance()->bufToStr((wxUint8*)tleft);

	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Ban detail: ") + detail);
}
/*-----------------------------------------------------------------------------------*/
void GRLoginParser::serverMessage(GR_PACKET *Packet)
{
	wxString message = GRUtil::getInstance()->bufToStr(Packet->payload);
	wxUint32 x;
	
	/* replace the 0x0d's with 0x0a's(linux doesen't accept the carriage return well */
	for(x = 0; x < message.Len(); x++) 
	{
		if(message[x] == 0x0d) message[x] = 0x0a;
	}

	GRMainWindow *mainWindow = GRApplication::getInstance()->getMainWindow();
	if(mainWindow != NULL) mainWindow->addTextWithColor(message+wxT("\n"), *wxRED);
}
/*-----------------------------------------------------------------------------------*/
void GRLoginParser::myUserInfo(GR_PACKET *Packet)
{
	wxUint8 *payload;
	wxUint32 userID;
	wxString nick, name, email;
	GRUtil *util = GRUtil::getInstance();

	payload = Packet->payload;

	payload += sizeof(wxUint32); //unknown

	//user id
	memcpy(&userID, payload, sizeof(wxUint32));
	userID = ntohl(userID);
	payload += sizeof(wxUint32);

	//email
	email = util->bufToStr(payload);
	payload += email.Len() + 1;

	//nickname
	nick = util->bufToStr(payload);
	payload += nick.Len() + 1;


	//real name
	name = util->bufToStr(payload);
	payload += name.Len() + 1;

	conn->getProfile()->grID = userID;
	conn->getProfile()->nickname = nick;
	conn->getProfile()->realname = name;
}	
/*-----------------------------------------------------------------------------------*/





