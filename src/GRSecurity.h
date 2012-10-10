/*
GameRanger
GRSecurity.h

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

#ifndef GRSECURITY_H
#define GRSECURITY_H

#include "GRBaseDefs.h"

//Client Verify Code
wxUint32 encryptVerifyCode(wxUint32 code);

//Algorithms and r6 memory
void lwzx(wxUint32 * r0);
wxUint32 Algorithm(char *code, char *MAC, int ID, int LAN_IP, int type);

#endif
