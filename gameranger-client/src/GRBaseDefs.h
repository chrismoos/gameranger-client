/*
GameRanger
GRBaseDefs.h

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

#ifndef GRBASEDEFS_H
#define GRBASEDEFS_H

#include "GRProtocol.h"

//#define SHOW_LOG_WINDOW

#include <wx/wxprec.h>
#include <wx/file.h>
#include <wx/wizard.h>
#include <wx/socket.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/image.h>
#include <wx/timer.h>
#include <vector>
#include <algorithm>

#ifndef WIN32
#include <netinet/in.h>
#else

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#endif
