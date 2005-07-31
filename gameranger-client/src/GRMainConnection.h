/*
GameRanger
GRMainConnection.h

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

#ifndef GRMAINCONNECTION_H
#define GRMAINCONNECTION_H

#include "GRBaseDefs.h"
#include "GRConnection.h"
#include "GRClientAuth.h"
#include "GRErrorParser.h"
#include "GRProfile.h"
#include "GRSecurity.h"

class GRLobbyManager;
class GRLoginParser;
class GRGameParser;
class GRPremiumUserInfoWindow;
class GRLobbyParser;
class GRGameManager;

class GRMainConnection : public GRConnection {
public:
	GRMainConnection();
	~GRMainConnection();

	/* Events */
	void OnConnect(); 
	void OnUnableToConnect();
	void OnDisconnect();

	/* Handle packet */
	void handlePacket(GR_PACKET *Packet);

	/* Set Login Parameters */
	void setLoginParams(GRProfile *profile);
	void setLoginParams(wxString email, wxString pass, bool savePass);

	/* Login plugins list */
	wxUint8 *makePluginsList();

	/* Lobby Manager */
	GRLobbyManager *getLobbyManager();

	/* Game Manager */
	GRGameManager *getGameManager();

	/* Login to GR */
	void loginToGR(GR_PACKET *Packet);
	
	/* Get login profile */
	GRProfile *getProfile();

	/* User Info */
	void regularUserInfo(GR_PACKET *Packet);
	void premiumUserInfo(GR_PACKET *Packet);
	void requestPicture(wxUint32 pictureID);
	void recvPremiumUserImage(GR_PACKET *Packet);
	void premiumInfoWindowClosed();

	/* Generic Icon Stuff */
	void requestIcon(wxUint32 iconID);
	void userChangedIcon(GR_PACKET *Packet);
	void receivedIconData(GR_PACKET *Packet);
	void purgeIcons();

private:
	/* Parsers */
	GRErrorParser *errorParser;
	GRLoginParser *loginParser;
	GRLobbyParser *lobbyParser;
	GRGameParser *gameParser;

	/* Connection-Specific Managers */
	GRLobbyManager *lobbyManager;
	GRGameManager *gameManager;

	/* Premium Info Window */
	GRPremiumUserInfoWindow *currentPremiumInfoWindow;

	/* Login Profile */
	GRProfile *loginProfile;

	/* Login email and pass for first login */
	wxString email;
	wxString pass;
};

#endif
