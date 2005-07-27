/*
GameRanger
GRProtocol.h

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

#ifndef GRPROTOCOL_H
#define GRPROTOCOL_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#define SERVER_HOSTNAME "master.gameranger.com"
#define SERVER_PORT 16000
#define OFFLINE 0
//================================================
//Command List
//===============================================
#define CLIENT_VERIFY 0x01
#define CLIENT_VERIFY_ACK 0x02
#define READY_TO_PROCESS 0x04
#define CREATE_ACCOUNT 0x05
#define CONFIRM_ACCOUNT_ACK 0x07
#define LOGIN_TO_GAMERANGER 0x08
#define JOIN_PUBLIC_LOBBY 0x10
#define BAN_TIME_LEFT 0x0A
#define APP_BANNER 0x0B
#define USER_LEFT_SERVER 0x0D
#define PUBLIC_LOBBY_LIST 0x0F
#define LOBBY_USER_LIST 0x13
#define GAME_ROOM_OPENED 0x19
#define GAME_ROOM_CLOSED 0x1A
#define JOIN_GAME_ROOM 0x1B
#define GAME_ROOM_FULL 0x1D
#define GAME_ROOM_INVALID_PASSWORD 0x1E
#define GAME_ROOM_USER_LIST 0x1F
#define PLAYER_JOINED_GAME_ROOM 0x20
#define PLAYER_LEFT_GAME_ROOM 0x21
#define PLAYER_JOINED_MY_GAME_ROOM 0x22
#define PLAYER_LEFT_MY_GAME_ROOM 0x23
#define HOST_GAME_ROOM 0x24
#define LEAVE_GAME_ROOM 0x27
#define SEND_LOBBY_CHAT_MESSAGE 0x28
#define LOBBY_CHAT_MESSAGE 0x29
#define LOBBY_CHAT_MESSAGE_ME 0x2B
#define SEND_GAME_ROOM_MESSAGE 0x2C
#define GAME_ROOM_CHAT_MESSAGE 0x2D
#define SEND_PRIVATE_MESSAGE 0x30
#define RCV_PRIVATE_MESSAGE 0x32
#define CHANGE_NICKNAME 0x35
#define USER_CHANGED_NICKNAME 0x36
#define REQUEST_ICON_DATA 0x37
#define RECEIVED_ICON_DATA 0x38
#define CHANGE_ICON 0x39
#define USER_CHANGED_ICON 0x3A
#define CHANGE_REAL_NAME 0x3B
#define GET_USER_INFO 0x3E
#define REG_USER_INFO 0x40
#define USER_IS_IDLE 0x43
#define USER_IS_ACTIVE 0x44
#define GAME_LAUNCH_LOADING 0x57
#define GAME_ROOM_IS_LOADING 0x58
#define GAME_LAUNCH_DONE 0x59
#define GAME_ROOM_LAUNCHED 0x5A
#define ABORT_GAME_ROOM 0x5B
#define GAME_ROOM_NO_LATE_JOINERS 0x5E
#define GAME_ROOM_STATUS_CHANGED 0x5F
#define CHANGE_ACCOUNT_PASSWORD 0x52
#define PASSWORD_CHANGE_SUCCESSFUL 0x53
#define PM_ERROR_USER_IN_GAME 0x5D
#define SERVER_MESSAGE 0x62
#define USER_IN_GAME 0x63
#define USER_NOT_IN_GAME 0x64
#define FIND_USER 0x6D
#define FIND_USER_RESULTS 0x6E
#define GR_ALIVE_PULSE 0x9B
#define LOBBY_USER_ACTION 0xB5
#define GAME_ROOMS_LIST 0xBC
#define MY_USER_INFO 0xBD
#define CONFIRM_EMAIL_ADDRESS 0xC5
#define INVALID_ACCOUNT 0xC8
#define INVALID_LOGIN_PASSWORD 0xCA
#define SERVER_VERIFY_ACK 0xD0
#define PREMIUM_USER_INFO 0xD1
#define GET_PREMIUM_USER_IMAGE 0xD2
#define RECV_PREMIUM_USER_IMAGE 0xD3
#define GR_LADDERS 0xF9

//=================================================
//Find User Search Types
//=================================================
#define SEARCH_BY_ID 0x00
#define SEARCH_BY_EMAIL 0x01
#define SEARCH_BY_NICK 0x02
#define SEARCH_BY_ACCOUNT 0x03

//=================================================
//User Status
//=================================================

#define REGULAR_NOT_IDLE 0
#define REGULAR_IDLE 1
#define PREMIUM_NOT_IDLE 2
#define PREMIUM_IDLE 3
#define REGULAR_PLAYING_NOT_IDLE 8
#define REGULAR_PLAYING_IDLE 9
#define PREMIUM_PLAYING_NOT_IDLE 10
#define PREMIUM_PLAYING_IDLE 11

//=================================================
//Game Room Status
//=================================================
#define NOT_LOCKED_NO_LATE 0x00
#define LOCKED_NO_LATE 0x01
#define NOT_LOCKED_LATE 0x02
#define LOCKED_LATE 0x03
#define PLAYING_NOT_LOCKED 0x04
#define PLAYING_LOCKED_NO_LATE 0x05
#define PLAYING_NOT_LOCKED_LATE 0x06
#define PLAYING_LOCKED_LATE 0x07


//=================================================
//Packet Structure
//=================================================
struct GR_PACKET_HEADER {
	wxUint32 command;
	wxUint32 payloadLength;
};

struct GR_PACKET {
	GR_PACKET_HEADER *header;
	wxUint8 *payload;
};
//================================================
//Client Auth Packet Structure
//=================================================
struct GR_CLIENT_AUTH_PACKET {
	wxUint32 version;
	wxUint32 version1;
	wxUint32 code;
	wxUint32 version2;
	wxUint32 version3;
};
//==================================================
//Client Auth Ack Packet Structure 
//==================================================
struct GR_CLIENT_AUTH_ACK_PACKET {
	wxUint32 clientVersion;
	wxUint32 verifyCode;
	wxUint32 padding;
	wxUint32 srvVerifyCode;
};
//==================================================
//Confirm Account Ack Packet Structure
//==================================================
struct GR_CONFIRM_ACCOUNT_ACK_PACKET {
	wxUint32 responseCode;
};

//==================================================
//Create Account Header Structure 
//==================================================
struct GR_CREATE_ACCOUNT_HEADER {
	wxUint32 checksum;
	wxUint32 padding;
	wxUint32 profanityFilter;
	wxUint32 iconID;
};
//==================================================
//Create Account Ack Packet Structure
//==================================================
struct GR_CREATE_ACCOUNT_ACK_PACKET {
	wxUint32 responseCode;
};

//==================================================
//Login Packet Structure
//==================================================
struct GR_LOGIN_PACKET {
	wxUint8 loginType;
	wxUint32 previousGRID;
	wxUint8 macAddress[6];
	wxUint8 lanIP[4];
	wxUint32 checksum;
};

//==================================================
//Ban time left Packet Structure
//==================================================
struct GR_BAN_TIME_PACKET {
	wxUint32 time_left;
};

//==================================================
//Game Room Packet Structure
//==================================================
struct GR_GAME_ROOM_PACKET {
	wxUint32 gameRoomID;
	wxUint32 gameCode;
	wxUint32 currentPlayers;
	wxUint32 maxPlayers;
	wxUint8 ipAddress[4];
	wxUint32 padding;
};

//==================================================
//User Info Packet Header
//==================================================
struct GR_USER_INFO_HEADER {
	wxUint32 userID;
	wxUint32 lastLogin;
	wxUint32 lastLogout;
	wxUint32 idleSince;
	wxUint32 iconID;
	wxUint8 location[12];
};

//==================================================
//User Changed Icon Packet
//==================================================
struct GR_USER_CHANGED_ICON_PACKET {
	wxUint32 userID;
	wxUint32 iconID;
};

//==================================================
//Game Room Status Changed Packet
//==================================================
struct GR_ROOM_STATUS_CHANGED {
	wxUint32 roomID;
	wxUint32 status;
};

//==================================================
//Host a game room
//==================================================
struct GR_HOST_GAME_ROOM {
	wxUint32 gameID;
	wxUint32 maxPlayers;
	wxUint32 unknown; /* 0xffffff */
	wxUint32 lateJoiners;
};




#endif
