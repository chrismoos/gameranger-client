/*
GameRanger
GRMainConnection.cpp

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

#include <wx/mstream.h>
#include "GRMainConnection.h"
#include "GRConnectStatusWindow.h"
#include "GRLogger.h"
#include "GRPluginManager.h"
#include "GRGameParser.h"
#include "GRLobbyManager.h"
#include "GRMainWindow.h"
#include "GRUserInfoWindow.h"
#include "GRPremiumUserInfoWindow.h"
#include "GRLoginParser.h"
#include "GRGameManager.h"
#include "GRIcon.h"
#include "GRApplication.h"
#include "GRFindPlayerWindow.h"
#include "GRPluginManager.h"
#include "GRLobbyParser.h"
#include "GRLoginWindow.h"
#include "GRUtil.h"

GRMainConnection::GRMainConnection()
{
	loginProfile = NULL;
	email = wxString(wxT(""));
	pass = wxString(wxT(""));

	/* Init parsers */
	errorParser = new GRErrorParser(this);
	loginParser = new GRLoginParser(this);
	lobbyParser = new GRLobbyParser(this);
	gameParser = new GRGameParser(this);

	/* Init connection specific managers */
	lobbyManager = new GRLobbyManager(this);
	gameManager = new GRGameManager(this);

	/* Init premium info window */
	currentPremiumInfoWindow = NULL;
}
/*-----------------------------------------------------------------------------------*/
GRMainConnection::~GRMainConnection()
{
	/* Cleanup parsers */
	delete(errorParser);
	delete(loginParser);
	delete(lobbyParser);
	delete(gameParser);

	/* Cleanup managers */
	delete(lobbyManager);
	delete(gameManager);

	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("GameRanger Connection successfully shut down."));
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::OnConnect()
{
	GRConnection::OnConnect();
	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Connected to GameRanger Master Server"));
	GRApplication::getInstance()->getMainWindow()->enableMenus();
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::OnDisconnect()
{
	GRConnection::OnDisconnect();
	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Disconnected GameRanger Master Server"));
	GRApplication::getInstance()->getMainWindow()->OnDisconnect();
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::OnUnableToConnect()
{
	GRConnection::OnUnableToConnect();
	GRLogger::getInstance()->log(GRLogger::LOG_INFO, wxT("Unable to connect to GameRanger Master Server"));
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::handlePacket(GR_PACKET *Packet)
{
	GRConnection::handlePacket(Packet);

	/* Run Parsers */
	if(errorParser->run(Packet)) return;
	if(loginParser->run(Packet)) return;
	if(lobbyParser->run(Packet)) return;
	if(gameParser->run(Packet)) return;

	/* Generic parse */
	switch(Packet->header->command) {
		case RECEIVED_ICON_DATA:
			receivedIconData(Packet);
		break;
		
		case USER_CHANGED_ICON:
			userChangedIcon(Packet);
		break;

		case RCV_PRIVATE_MESSAGE:
			GRApplication::getInstance()->getMainWindow()->receivedPrivateMessage(Packet);
		break;

		case FIND_USER_RESULTS:
			GRFindPlayerWindow::getInstance()->findUserResults(Packet);
		break;

		case REG_USER_INFO:
			regularUserInfo(Packet);
		break;

		case PREMIUM_USER_INFO:
			premiumUserInfo(Packet);
		break;

		case RECV_PREMIUM_USER_IMAGE:
			recvPremiumUserImage(Packet);
		break;

		default: 
			GRLogger::getInstance()->log(GRLogger::LOG_WARNING, wxT("Unhandled packet exception: Command: ") + wxString::Format(wxT("%08X\n"), Packet->header->command));
		break;
	}	
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::recvPremiumUserImage(GR_PACKET *Packet)
{
	wxUint8 *ptr;
	wxImage *img = new wxImage();
	wxMemoryInputStream *buf;

	ptr = Packet->payload + sizeof(wxUint32);

	buf = new wxMemoryInputStream(ptr,Packet->header->payloadLength - sizeof(wxUint32));
	img->LoadFile(*buf);

	if(currentPremiumInfoWindow != NULL)
	{	
		currentPremiumInfoWindow->SetImage(img);
	}	

	delete(buf);
	delete(img);
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::premiumInfoWindowClosed()
{
	currentPremiumInfoWindow = NULL;
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::premiumUserInfo(GR_PACKET *Packet)
{
	wxUint32 roomID, pictureID, memberSince;
	wxString nickname, realName, email, accountName, webSite, quote;
	wxUint16 favGame;
	wxUint8 *ptr;
	GR_USER_INFO_HEADER infoHeader;
	GRPremiumUserInfo *userInfo;
	GRPremiumUserInfoWindow *infoWindow;

	ptr = Packet->payload;

	//user info header
	memcpy(&infoHeader, ptr, sizeof(infoHeader));
	ptr += 32; //sizeof(GR_USER_INFO_HEADER)

	infoHeader.iconID = wxUINT32_SWAP_ON_LE(infoHeader.iconID);
	infoHeader.idleSince = wxUINT32_SWAP_ON_LE(infoHeader.idleSince);
	infoHeader.lastLogin = wxUINT32_SWAP_ON_LE(infoHeader.lastLogin);
	infoHeader.lastLogout = wxUINT32_SWAP_ON_LE(infoHeader.lastLogout);
	infoHeader.userID = wxUINT32_SWAP_ON_LE(infoHeader.userID);

	//nickname
	nickname = GRUtil::getInstance()->bufToStr(ptr);
	ptr += nickname.Len() + 1;

	//real name
	realName = GRUtil::getInstance()->bufToStr(ptr);
	ptr += realName.Len() + 1;

	//email
	email = GRUtil::getInstance()->bufToStr(ptr);
	ptr += email.Len() + 1;

	//unknown 
	ptr += 12;

	//current room id
	memcpy(&roomID, ptr, sizeof(wxUint32));
	roomID = wxUINT32_SWAP_ON_LE(roomID);
	ptr += sizeof(wxUint32);

	//account name
	accountName = GRUtil::getInstance()->bufToStr(ptr);
	ptr += accountName.Len() + 1;

	//member since
	memcpy(&memberSince, ptr, sizeof(wxUint32));
	memberSince = wxUINT32_SWAP_ON_LE(memberSince);
	ptr += sizeof(wxUint32);

	//webpage
	webSite = GRUtil::getInstance()->bufToStr(ptr);
	ptr += webSite.Len() + 1;

	//quote
	quote = GRUtil::getInstance()->bufToStr(ptr);
	ptr += quote.Len() + 1;

	//fav game
	memcpy(&favGame, ptr, sizeof(wxUint16));
	favGame = wxUINT16_SWAP_ON_LE(favGame);
	ptr += sizeof(wxUint16);

	//picture id
	memcpy(&pictureID, ptr, sizeof(wxUint32));
	pictureID = wxUINT32_SWAP_ON_LE(pictureID);
	ptr += sizeof(wxUint32);


	userInfo = new GRPremiumUserInfo(nickname, realName, infoHeader.userID, infoHeader.lastLogin, infoHeader.lastLogout,
		infoHeader.idleSince, infoHeader.iconID, roomID, infoHeader.location, lobbyManager->findLobby(roomID), GRIconCache::getInstance()->findIcon(infoHeader.iconID), email,
		accountName, webSite, quote, memberSince, pictureID, GRPluginManager::getInstance()->findPluginByCode(favGame));

	requestPicture(pictureID);

	infoWindow = new GRPremiumUserInfoWindow(GRApplication::getInstance()->getMainWindow(), wxT("User Info - ")+nickname, wxDefaultPosition, wxDefaultSize);

	infoWindow->SetInfo(userInfo);
	infoWindow->Show(true);
	currentPremiumInfoWindow = infoWindow;

}
/*------------------------------------------------------------------------------------------------------------*/
void GRMainConnection::requestPicture(wxUint32 pictureID)
{
	wxUint32 picture;

	picture = pictureID;
	picture = wxUINT32_SWAP_ON_LE(picture);

	sendGRPacket(GET_PREMIUM_USER_IMAGE, sizeof(wxUint32), (wxUint8*)&picture);
}
/*-----------------------------------------------------------------------------------------------------------*/
void GRMainConnection::regularUserInfo(GR_PACKET *Packet)
{
	wxUint32 roomID;
	wxString nickname, realName, email;
	wxUint8 *ptr;
	GR_USER_INFO_HEADER infoHeader;
	GRUserInfo *userInfo;
	GRUserInfoWindow *infoWindow;

	ptr = Packet->payload;

	//user info header
	memcpy(&infoHeader, ptr, sizeof(infoHeader));
	ptr += 32; //sizeof(GR_USER_INFO_HEADER)

	infoHeader.iconID = wxUINT32_SWAP_ON_LE(infoHeader.iconID);
	infoHeader.idleSince = wxUINT32_SWAP_ON_LE(infoHeader.idleSince);
	infoHeader.lastLogin = wxUINT32_SWAP_ON_LE(infoHeader.lastLogin);
	infoHeader.lastLogout = wxUINT32_SWAP_ON_LE(infoHeader.lastLogout);
	infoHeader.userID = wxUINT32_SWAP_ON_LE(infoHeader.userID);

	//nickname
	nickname = GRUtil::getInstance()->bufToStr(ptr);
	ptr += nickname.Len() + 1;

	//real name
	realName = GRUtil::getInstance()->bufToStr(ptr);
	ptr += realName.Len() + 1;

	//email
	email = GRUtil::getInstance()->bufToStr(ptr);
	ptr += email.Len() + 1;

	//unknown 
	ptr += 12;

	//current room id
	memcpy(&roomID, ptr, sizeof(wxUint32));
	roomID = wxUINT32_SWAP_ON_LE(roomID);

	userInfo = new GRUserInfo(nickname, realName, infoHeader.userID, infoHeader.lastLogin, infoHeader.lastLogout,
		infoHeader.idleSince, infoHeader.iconID, roomID, infoHeader.location, lobbyManager->findLobby(roomID), GRIconCache::getInstance()->findIcon(infoHeader.iconID), email);

	infoWindow = new GRUserInfoWindow(GRApplication::getInstance()->getMainWindow(), wxT("User Info - ")+nickname, wxDefaultPosition, wxDefaultSize);

	infoWindow->SetInfo(userInfo);
	infoWindow->Show(true);

}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::userChangedIcon(GR_PACKET *Packet)
{
	GR_USER_CHANGED_ICON_PACKET *pck;
	GRIcon *icon;
	GRUser *user;
	int imageIndex;
	pck = (GR_USER_CHANGED_ICON_PACKET*)Packet->payload;

	pck->userID = wxUINT32_SWAP_ON_LE(pck->userID);
	pck->iconID = wxUINT32_SWAP_ON_LE(pck->iconID);

	/* See if the change was in the lobby */
	lobbyManager->getCurrentLobby()->userChangedIcon(pck->userID, pck->iconID);

	/* Check if we have this in the cache, if not, request it */
	if(GRIconCache::getInstance()->findIcon(pck->iconID) == NULL) {
		requestIcon(pck->iconID);
	}
}
/*-------------------------------------------------------------------------------------*/
void GRMainConnection::requestIcon(wxUint32 iconID)
{
	wxUint32 len = 0, pos = 0, count;
	wxUint8 *buf;

	count = 1;
	count = htonl(count);


	len = sizeof(wxUint32)*2;

	buf = new wxUint8[len];

	memcpy(buf, (wxUint8*)&count, sizeof(wxUint32));
	pos += sizeof(wxUint32);

	iconID = htonl(iconID);
	memcpy(buf+pos, (wxUint8*)&iconID, sizeof(wxUint32));
	pos += sizeof(wxUint32);

	sendGRPacket(REQUEST_ICON_DATA, len, buf);
	delete[] buf;
}
/*------------------------------------------------------------------------------*/
void GRMainConnection::receivedIconData(GR_PACKET *Packet)
{
	wxUint32 count, x, iconID;
	wxUint8 *ptr;
	GRIcon *icon;
	GRMainWindow *mainWindow = GRApplication::getInstance()->getMainWindow();
	GRIconCache *iconCache = GRIconCache::getInstance();

	ptr = Packet->payload;
	
	memcpy(&count, ptr, sizeof(wxUint32));
	count = ntohl(count);
	ptr += sizeof(wxUint32);

	for(x = 0; x < count; x++)
	{
		memcpy(&iconID, ptr, sizeof(wxUint32));
		iconID = ntohl(iconID);
		ptr += sizeof(wxUint32);
	
		icon = new GRIcon();
		icon->SetIconID(iconID);
		icon->SetIconData(ptr);
		icon->imageIndex = mainWindow->iconImgList->Add(*icon->image);
		iconCache->AddIcon(icon);

		ptr += 288;
	}
	purgeIcons();
}
/*-----------------------------------------------------------------------------*/
void GRMainConnection::purgeIcons()
{
	lobbyManager->getCurrentLobby()->purgeIcons();

	/* TODO - update in game rooms, private messages, and aynthing else that might need it */
}
/*----------------------------------------------------------------------------------*/
void GRMainConnection::setLoginParams(GRProfile *profile)
{
	loginProfile = profile;
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::setLoginParams(wxString email, wxString pass, bool savePass)
{
	loginProfile = new GRProfile();
	loginProfile->email = email;
	loginProfile->password = pass;
	loginProfile->savePass = savePass;
	GRProfileManager::getInstance()->addProfile(loginProfile);
}
/*-----------------------------------------------------------------------------------*/
GRProfile *GRMainConnection::getProfile()
{
	if(loginProfile == NULL) {
		loginProfile = new GRProfile();
	}
	return loginProfile;
}
/*-----------------------------------------------------------------------------------*/
GRLobbyManager *GRMainConnection::getLobbyManager()
{
	if(lobbyManager == NULL) {
		lobbyManager = new GRLobbyManager(this);
	}
	return lobbyManager;
}
/*-----------------------------------------------------------------------------------*/
GRGameManager *GRMainConnection::getGameManager()
{
	if(gameManager == NULL) {
		gameManager = new GRGameManager(this);
	}
	return gameManager;
}
/*-----------------------------------------------------------------------------------*/
void GRMainConnection::loginToGR(GR_PACKET *Packet)
{
	GR_LOGIN_PACKET loginPacketHeader;
	wxUint32 ip = 0x7f000001;
	wxUint32 GRID;
	wxUint32 len = 0, pos = 0;
	wxUint8 *payload = NULL;
	bool emailLogin;
	GRProfile *currentProfile = getProfile();

	if(currentProfile->grID == 0) emailLogin = true;
	else emailLogin = false;

	char *GRpassword = (char*)(const char*)currentProfile->password.mb_str();
	char *GRemail = (char*)(const char*)currentProfile->email.mb_str();

	char temp[1];
	const char *options = "\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	//options[3] = 0x08;
    const char *location = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF"; //location
		
    const char *staticBuf = "\x90\x09\x21\x00\x00\x00\x00";

	wxUint8 *plgList = makePluginsList();

	GRConnectStatusWindow::getInstance(GRApplication::getInstance()->getMainWindow())->SetTitle(wxT("Logging in..."));

	if(emailLogin == true) loginPacketHeader.loginType = 2; //email based login
	else loginPacketHeader.loginType = 1;

	GRID = htonl(currentProfile->grID);
	if(emailLogin == true) loginPacketHeader.previousGRID = htonl(0);
	else loginPacketHeader.previousGRID = GRID;

	//mac
	loginPacketHeader.macAddress[0] = currentProfile->macAddress[0];
	loginPacketHeader.macAddress[1] = currentProfile->macAddress[1];
	loginPacketHeader.macAddress[2] = currentProfile->macAddress[2];
	loginPacketHeader.macAddress[3] = currentProfile->macAddress[3];
	loginPacketHeader.macAddress[4] = currentProfile->macAddress[4];
	loginPacketHeader.macAddress[5] = currentProfile->macAddress[5];

	//xor each byte
	for(wxUint32 x = 0; x < 6; x++) loginPacketHeader.macAddress[x] ^= 0x77;

	int verifyCode = GRConnection::getClientAuth()->getVerifyCode();
	if(emailLogin == true) loginPacketHeader.checksum = htonl(Algorithm((char*)&verifyCode, (char*)&loginPacketHeader.macAddress, 0, ip, 2));
	else loginPacketHeader.checksum = htonl(Algorithm((char*)&verifyCode, (char*)&loginPacketHeader.macAddress, ntohl(GRID), ip, 2));
	ip = htonl(ip);
	ip ^= 0xffffffff;

	//construct packet
	len += sizeof(wxUint8); //login type
	len += sizeof(wxUint32); //gr id
	len += 6; //mac address size
	len += 4; //lan ip
	len += sizeof(wxUint32); //checksum

	len += strlen(GRpassword) + 1;
	
	if(emailLogin == true) len += strlen(currentProfile->email) + 1;
	else len += sizeof(wxUint32); //gr id

	len += 16; //options
	if(*currentProfile->gamesList != 0)
		len += *(currentProfile->gamesList)+1;
	len += 12; //location
	len += ((GRPluginManager::getInstance()->getPluginCount()-2)*6)+9; //plugins
	len += sizeof(wxUint32); //end thing

	payload = new wxUint8[len];

	/* construct packet */

	//login type
	memcpy(payload+pos, &loginPacketHeader.loginType, 1);
	pos += 1;

	//previous gr id
	memcpy(payload+pos, &loginPacketHeader.previousGRID, sizeof(wxUint32));
	pos += sizeof(wxUint32);

	//mac address
	memcpy(payload+pos, &loginPacketHeader.macAddress, 6); 
	pos += 6;

	//lan ip
	memcpy(payload+pos, &ip, 4);
	pos += 4;

	//checksum
	memcpy(payload+pos, &loginPacketHeader.checksum, sizeof(wxUint32));
	pos += sizeof(wxUint32);
	
	//password 
	temp[0] = (wxUint8)strlen(GRpassword);
	memcpy(payload+pos, temp, 1);
	pos++;
	memcpy(payload+pos, GRpassword, strlen(GRpassword));
	pos += strlen(GRpassword);

	if(emailLogin == true) {
		//email
		temp[0] = (wxUint8)strlen(GRemail);
		memcpy(payload+pos, temp, 1);
		pos++;
		memcpy(payload+pos, GRemail, strlen(GRemail));
		pos += strlen(GRemail);
	}
	else {
		//gr id
		memcpy(payload+pos, &GRID, sizeof(wxUint32));
		pos += sizeof(wxUint32);
	}

	//options
	memcpy(payload+pos, options, 16);
	pos += 16;

	//games list
	if(*currentProfile->gamesList == 0)
	{
		//do nothing
	}
	else
	{
		*currentProfile->gamesList = *currentProfile->gamesList + 1;
		memcpy(payload+pos, currentProfile->gamesList, *(currentProfile->gamesList));
		pos += *(currentProfile->gamesList);
		*currentProfile->gamesList = *currentProfile->gamesList - 1;
	}

	//location
	memcpy(payload+pos, location, 12);
	pos += 12;

	//unknown static buf
	memcpy(payload+pos, staticBuf, 7);
	pos += 7;

	//plugins
	memcpy(payload+pos, plgList, ((GRPluginManager::getInstance()->getPluginCount()-2)*6)+2);
	pos += ((GRPluginManager::getInstance()->getPluginCount()-2)*6)+2;

	//from last packet, copy to end
	memcpy(payload+pos, Packet->payload, 4);
	pos += 4;

	//logWindow->logData(payload, pos);

	sendGRPacket(LOGIN_TO_GAMERANGER, pos, payload);

	delete[] plgList;
	delete[] payload;
}
/*-----------------------------------------------------------------------------------*/
wxUint8 *GRMainConnection::makePluginsList()
{
	wxUint32 len = 0, pluginCount = 0;
	wxUint8 *pluginList = NULL;
	wxUint16 gameCode;
	wxUint16 count;
	int *array;
	unsigned int x, y, pos = 0;

	//count of plugins
	pluginCount = GRPluginManager::getInstance()->getPluginCount();

	//calculate lengths, each plugin uses 6 bytes, 2 for game code, 4 for options
	len = ((pluginCount-2) * 6)+2; //we have chat in there

	//allocate memory
	pluginList = new wxUint8[len];

	//make our array and fill it
	array = new int[pluginCount];
	for(x = 0; x < GRPluginManager::getInstance()->getPluginCount(); x++)
	{
		array[x] = GRPluginManager::getInstance()->getPluginAt(x)->gameCode;
	}

	//sort it
	sort(array, array+pluginCount);

	//count
	count = pluginCount-2;
	count = wxUINT16_SWAP_ON_LE(count);
	memcpy(pluginList+pos, &count, 2);
	pos += 2;

	//let's make packet
	for(y = 0; y < pluginCount; y++)
	{
		for(x = 0; x < GRPluginManager::getInstance()->getPluginCount(); x++)
		{
			if(array[y] == 0) continue;
			if(array[y] == 0xffffffff) continue;
			if(GRPluginManager::getInstance()->getPluginAt(x)->gameCode == array[y])
			{
				gameCode = GRPluginManager::getInstance()->getPluginAt(x)->gameCode;
				gameCode = wxUINT16_SWAP_ON_LE(gameCode);
				memcpy(pluginList+pos, &gameCode, sizeof(wxUint16));
				pos += sizeof(wxUint16);
				memcpy(pluginList+pos, GRPluginManager::getInstance()->getPluginAt(x)->unknownOption, 4);
				pos += 4;
				break;
			}
		}
	}
	//logWindow->logData(pluginList, len);

	return pluginList;
}
/*-----------------------------------------------------------------------------------*/
