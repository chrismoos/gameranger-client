/*
GameRanger
GRConnection.cpp

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

#include "GRConnection.h"
#include "GRConnectStatusWindow.h"
#include "GRLogger.h"
#include "GRApplication.h"

GRConnection::GRConnection()
{
	/* socket bs */
	connecting = false;
	socket = new wxSocketClient();
	socket->SetEventHandler(*this, GRSOCKETID);
	socket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_CONNECTION_FLAG | wxSOCKET_LOST_FLAG);
	socket->Notify(true);

	/* timer */
	timer = new wxTimer(this, GRTIMER_ID);

	/* Parsers */
	clientAuth = new GRClientAuth(this);
}
/*------------------------------------------------------------------------------*/
GRConnection::~GRConnection()
{
	/* Clean up parsers */
	delete(clientAuth);

	/* Clean up socket */
	socket->Destroy();
}
/*------------------------------------------------------------------------------*/
void GRConnection::OnSocketEvent(wxSocketEvent& event)
{
	auto socket = event.GetSocketEvent();
	switch(socket)
	{
		case wxSOCKET_CONNECTION:
			OnConnect();
		break;

		case wxSOCKET_LOST:
			if(connecting == true) OnUnableToConnect();
			else if(connecting == false) OnDisconnect();
		break;

		case wxSOCKET_INPUT:
			OnDataAvailable();
		break;
	}
}
/*------------------------------------------------------------------------------*/
void GRConnection::connectToGR()
{
	wxIPV4address addr;

	/* fill address struct in */
	addr.Hostname(wxT(SERVER_HOSTNAME));
	addr.Service(SERVER_PORT);

	/* open connecting window */
	GRConnectStatusWindow *statusWindow = GRConnectStatusWindow::getInstance(GRApplication::getInstance()->getMainWindow());
	statusWindow->Show(true);
	statusWindow->SetTitle(wxT("Connecting to GameRanger server..."));
	statusWindow->statusIndicator->SetRange(6);
	statusWindow->statusIndicator->SetValue(1);
	statusWindow->SetFocus();


	/* connect, and don't block */
	connecting = true;
	socket->SetFlags(wxSOCKET_WAITALL);
	socket->Connect(addr, false);
}
/*------------------------------------------------------------------------------*/
void GRConnection::OnDataAvailable()
{
	GR_PACKET_HEADER pckHeader;
	GR_PACKET Packet;
	wxUint32 pos = 0;
	wxUint8 *payload;

	/* make sure we haven't deleted socket(Socket->Close() is delayed) */
	if(socket == NULL) return;

	/* read in header */
	socket->Read(&pckHeader, sizeof(pckHeader));

	/* endian conversion */
	pckHeader.command = ntohl(pckHeader.command);
	pckHeader.payloadLength = ntohl(pckHeader.payloadLength);

	/* read in payload */
	payload = new wxUint8[pckHeader.payloadLength];

	pos = 0;
	for(;;)
	{
		socket->Read(payload+pos, pckHeader.payloadLength-pos);
		pos += socket->LastCount();
		if(pos == pckHeader.payloadLength) break;
	}

	if(socket->Error()) 
	{
		GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Error on socket. Not sure what to make out of it."));
	}

	/* put everything into our packet structure */
	Packet.header = &pckHeader;
	Packet.payload = payload;	


	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxString::Format(wxT("Command: %08X Payload Length: %d"), pckHeader.command, pckHeader.payloadLength));
	GRLogger::getInstance()->logData(Packet.payload, Packet.header->payloadLength);

	handlePacket(&Packet);

	delete[] payload;
}
/*------------------------------------------------------------------------------*/
void GRConnection::handlePacket(GR_PACKET *Packet)
{
	/* parse the packet */
	switch(Packet->header->command)
	{
		case CLIENT_VERIFY:
			clientAuth->clientVerify(Packet);
		break;

		case SERVER_VERIFY_ACK:
			GRConnectStatusWindow::getInstance(GRApplication::getInstance()->getMainWindow())->statusIndicator->SetValue(4);
		break;

		case READY_TO_PROCESS:
			GRConnectStatusWindow::getInstance(GRApplication::getInstance()->getMainWindow())->statusIndicator->SetValue(4);
		break;
	}
}
/*-----------------------------------------------------------------------------------*/
void GRConnection::OnConnect()
{
	GRConnectStatusWindow *statusWindow = GRConnectStatusWindow::getInstance(GRApplication::getInstance()->getMainWindow());
	connecting = false;
	statusWindow->SetTitle(wxT("Connected to GameRanger server..."));
	statusWindow->statusIndicator->SetValue(2);

	/* Packet counter */
	packetCounter = 0;

	/* start pulse timer */
	timer->Start(360000, false);
}
/*------------------------------------------------------------------------------*/
void GRConnection::OnDisconnect()
{
	/* close socket */
	socket->Close();

	/* stop pulse timer */
	timer->Stop();

	/* hide status window..*/
	GRConnectStatusWindow *statusWindow = GRConnectStatusWindow::getInstance(GRApplication::getInstance()->getMainWindow());
	statusWindow->Show(false);
}
/*------------------------------------------------------------------------------*/
void GRConnection::OnUnableToConnect()
{
	GRConnectStatusWindow *statusWindow = GRConnectStatusWindow::getInstance(GRApplication::getInstance()->getMainWindow());
	statusWindow->SetTitle(wxT("Disconnected from GameRanger server."));
	statusWindow->statusIndicator->SetValue(0);

	connecting = false;

	/* alert user we couldn't connect */
	wxMessageDialog *msgDialog = new wxMessageDialog(statusWindow, wxT("Unable to connect to the GameRanger server. Please check your connection and try again."), wxT("Connection error"), wxICON_ERROR);
	msgDialog->ShowModal();
	delete(msgDialog);
	statusWindow->Show(false);
}
/*-----------------------------------------------------------------------------------*/
void GRConnection::OnTimer(wxTimerEvent &evt)
{
	(void)evt;

	sendGRPacket(GR_ALIVE_PULSE, 0, NULL);
}
/*------------------------------------------------------------------------------*/
void GRConnection::sendGRPacket(wxUint32 command, wxUint32 payloadLength, wxUint8 *payload)
{
	GR_PACKET_HEADER header;
	wxUint32 len = 0;
	wxUint8 *packet;
	wxUint8 counterByte;
	bool writeCounterByte = false;

	header.command = htonl(command);

	writeCounterByte = writeCounter(command);
	
	if(writeCounterByte == false) header.payloadLength = htonl(payloadLength);
	else header.payloadLength = htonl(payloadLength+1);	
	

	//Check if we should append packet counter byte
	if(writeCounterByte == false) len = sizeof(header)+payloadLength;
	else len = sizeof(header)+payloadLength + sizeof(wxUint8);


	packet = new wxUint8[len];

	memcpy(packet, &header, sizeof(header));
	memcpy(packet+sizeof(header), payload, payloadLength);

	//append packet counter byte
	if(writeCounterByte == true) 
	{
		counterByte = getPacketCounterByte(command, payloadLength);
		memcpy(packet+sizeof(header)+payloadLength, (wxUint8*)&counterByte, sizeof(wxUint8));
	}
		
	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Sent Data"));
	GRLogger::getInstance()->logData(packet, len);

	if(socket->IsConnected())
		socket->Write(packet, len);

	delete[] packet;
}
/*------------------------------------------------------------------------------*/
bool GRConnection::writeCounter(wxUint32 command)
{
	switch(command)
	{
		case CLIENT_VERIFY_ACK:
			return false;
		break;

		case CREATE_ACCOUNT:
			return false;
		break;

		case CONFIRM_EMAIL_ADDRESS:
			return false;
		break;

		case LOGIN_TO_GAMERANGER:
			return false;
		break;

		case GR_ALIVE_PULSE:
			return false;
		break;

		default:
			return true;
		break;
	}
}
/*------------------------------------------------------------------------------*/
wxUint32 GRConnection::getPacketCounterByte(wxUint32 command, wxUint32 payloadLength)
{
	wxUint32 r0;

	if(packetCounter == 0) 
	{
		lastCommand = command;
		lastLength = payloadLength;
		packetCounter++;
		return 0;
	}

	r0 = lastCommand;
	r0 ^= lastLength;
	r0 ^= packetCounter;
	r0 &= 0x000000FF;

	lastCommand = command;
	lastLength = payloadLength;

	packetCounter++;
	return r0;
}
/*------------------------------------------------------------------------------*/
void GRConnection::testPacket(wxString filename)
{
	wxFile *file = new wxFile();
	GR_PACKET_HEADER header;
	GR_PACKET packet;
	wxUint8 *buf;
	wxUint32 len;

	if(!file->Exists(filename)) 
	{
		delete(file);
		return;
	}

	file->Open(filename, wxFile::read);

	len = file->Length();
	buf = new wxUint8[len];

	file->Read(buf, (wxUint32)len);

	memcpy(&header, buf, sizeof(GR_PACKET_HEADER));
	header.command = ntohl(header.command);
	header.payloadLength = ntohl(header.payloadLength);

	packet.header = &header;
	packet.payload = (wxUint8*)buf+sizeof(GR_PACKET_HEADER);

	handlePacket(&packet);

	file->Close();
	delete[] buf;
	delete(file);
}
/*------------------------------------------------------------------------------*/
GRClientAuth *GRConnection::getClientAuth()
{
	return clientAuth;
}
/*------------------------------------------------------------------------------*/
