/*
GameRanger
GRLogger.h

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

#ifndef GRLOGGER_H
#define GRLOGGER_H

#include "GRBaseDefs.h"
#include "GRLogWindow.h"

class GRLogger {
public:
	~GRLogger();
	static GRLogger *getInstance();

	/* Some types of logging */
	const static int LOG_INFO = 1;
	const static int LOG_WARNING = 2;
	const static int LOG_ERROR = 3;

	/* Log stuff */
	void log(int type, wxString msg);
	void logData(wxUint8 *buf, wxUint32 length);

	/* Log window stuff */
	void setWindowDestroyed();

	/* Logging mutex */
	static wxMutex logMutex;

protected:
	GRLogger();

private:
	static GRLogger *_instance;

	/* The Log Window */
	GRLogWindow *logWindow;
};

#endif
