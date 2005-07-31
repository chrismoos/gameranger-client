/*
GameRanger
GRClientAuth.cpp

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

#include "GRClientAuth.h"
#include "GRSecurity.h"
#include "GRConnectStatusWindow.h"
#include "GRLogger.h"
#include "GRConnection.h"

GRClientAuth::GRClientAuth(GRConnection *conn)
{
	this->conn = conn;
}
/*--------------------------------------------------------------------------*/
GRClientAuth::~GRClientAuth()
{

}
/*--------------------------------------------------------------------------*/
void GRClientAuth::clientVerify(GR_PACKET *Packet)
{
	GR_CLIENT_AUTH_PACKET *payload;
	GR_CLIENT_AUTH_ACK_PACKET authAckPacket;

	payload = (GR_CLIENT_AUTH_PACKET*)Packet->payload;

	/* convert endianess */
	payload->code = ntohl(payload->code);
	payload->version = ntohl(payload->version);
	payload->version1 = ntohl(payload->version1);
	payload->version2 = ntohl(payload->version2);
	payload->version3 = ntohl(payload->version3);

	/* emulate latest version log */
	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxString::Format(wxT("Emulating client version %08X..."), payload->version));
	
	//Craft Response
	authAckPacket.clientVersion = htonl(payload->version);
	authAckPacket.verifyCode = htonl(encryptVerifyCode(payload->code));
	authAckPacket.padding = htonl(0);
	authAckPacket.srvVerifyCode = htonl(rand());

	verifyCode = authAckPacket.verifyCode;

	conn->sendGRPacket(CLIENT_VERIFY_ACK, sizeof(authAckPacket), (wxUint8*)&authAckPacket);

	GRConnectStatusWindow::getInstance()->SetTitle(wxT("Checking versions..."));
	GRConnectStatusWindow::getInstance()->statusIndicator->SetValue(3);
}
/*--------------------------------------------------------------------------*/
int GRClientAuth::getVerifyCode()
{
	return verifyCode;
}
/*--------------------------------------------------------------------------*/
