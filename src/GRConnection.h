/*
GameRanger
GRConnection.h

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

#ifndef GRCONNECTION_H
#define GRCONNECTION_H

#include "GRBaseDefs.h"
#include "GRClientAuth.h"

class GRConnection : public wxEvtHandler {
public:
	GRConnection();
	~GRConnection();

	/* Socket Events */
	void OnSocketEvent(wxSocketEvent& event);
	void OnDataAvailable();
	
	/* Timer event */
	void OnTimer(wxTimerEvent &evt);

	/* Virtual methods */
	virtual void OnConnect(); 
	virtual void OnUnableToConnect();
	virtual void OnDisconnect();

	/* Send Packet */
	void sendGRPacket(wxUint32 command, wxUint32 payloadLength, wxUint8 *payload);

	/* Security Byte */
	bool writeCounter(wxUint32 command);
	wxUint32 getPacketCounterByte(wxUint32 command, wxUint32 payloadLength);

	/* GR Packet parsing(virtual), they must implement */
	virtual void handlePacket(GR_PACKET *Packet) =0;

	/* Get Parser */
	GRClientAuth *getClientAuth();

	/* Connect to GR */
	void connectToGR();

	/* Test Packet */
	void testPacket(wxString filename);

	DECLARE_EVENT_TABLE()

private:
	wxSocketClient *socket;
	bool connecting;

	/* Pulse timer */
	wxTimer *timer;

	/* Packet counter */
	int packetCounter;
	int lastCommand;
	int lastLength;

	/* Parsers */
	GRClientAuth *clientAuth;
};

enum {
	GRSOCKETID = wxID_HIGHEST + 500,
	GRTIMER_ID
};

#endif
