/*
GameRanger
GRChangeIconWindow.cpp

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

#include "GRChangeIconWindow.h"
#include "GRMainWindow.h"
#include "GRApplication.h"
#include "GRMainConnection.h"
#include "GRIcon.h"
#include "GRLogWindow.h"
#include "memdebug.h"

GRChangeIconWindow::GRChangeIconWindow(const wxFrame *parent,const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame((wxFrame *) parent, -1, title, pos, size)
{
	//Set some generic window options
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//Create controls
	createControls();

	//Center window
	CentreOnScreen();

	//null it
	m_img = NULL;
	m_mainWindow = NULL;
	plt = NULL;

}
//------------------------------------------------------------------------------------
GRChangeIconWindow::~GRChangeIconWindow()
{
	if(m_img != NULL) delete(m_img);
	if(plt != NULL) delete(plt);
}
//-------------------------------------------------------------------------------------
void GRChangeIconWindow::createControls()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *sizer1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *iconText = new wxStaticText(this, -1, wxT("Icon: "));
	

	m_imgLocation = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize(150,20));
	m_loadButton = new wxButton(this, LOAD_BUTTON_ID, wxT("Load Image"), wxDefaultPosition, wxDefaultSize);
	m_changeButton = new wxButton(this, CHANGE_ICON_BUTTON_ID, wxT("Change Icon"), wxDefaultPosition, wxDefaultSize);
	bmp = new wxStaticBitmap(this, -1, wxBitmap(wxNullIcon),wxDefaultPosition, wxSize(16,16));
	sizer->Add(m_imgLocation);
	sizer->Add(m_loadButton, 0, wxLEFT, 5);

	sizer1->Add(iconText, 0, wxALIGN_LEFT);
	sizer1->Add(bmp, 0, wxALIGN_LEFT);
	sizer1->Add(m_changeButton, 0, wxLEFT, 5);

	mainSizer->Add(sizer, 0, wxALL | wxALIGN_CENTER, 10);
	mainSizer->Add(sizer1, 0, wxALL, 10);


	//Activate Sizer
	SetAutoLayout(true);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);
}
//--------------------------------------------------------------------------------------
void GRChangeIconWindow::OnLoadIcon(wxCommandEvent &event)
{
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Locate an image"), wxT(""), wxT(""), wxT("*.*"), wxOPEN);
	wxString path;

	if(dlg->ShowModal() == wxID_CANCEL) return;
	
	
	path = dlg->GetPath();

	m_imgLocation->SetValue(path);
	m_img = new wxImage();

	m_imgLocation->SetValue(path);

	m_img->LoadFile(path);
	m_img->Rescale(16,16);
	if(plt == NULL)
	{
		plt = getMacPalette();
	}
	bmp->SetPalette(*plt);
	bmp->SetBitmap(wxBitmap(m_img));


	delete(plt);
	plt = NULL;
	delete(dlg);
}
//----------------------------------------------------------------------------------------
void GRChangeIconWindow::OnChangeIcon(wxCommandEvent &event)
{
	if(m_mainWindow == NULL) return;
	wxUint8 *payload;
	wxUint8 *iconData = new wxUint8[256];
	wxUint32 index;
	wxUint8 index1;
	plt = getMacPalette();
	wxImage *testImg = m_img;
	testImg->SetPalette(*plt);
	wxUint8 *padding = (wxUint8*)"\x00\x00\x00\x00\x00\x00\x00\x00\x7F\xFF\x7F\xFF\x7F\xFF\x7F\xFF\x7F\xFF\x7F\xFF\x7F\xFF\x7F\xFF\x7F\xFF\x00\x00\x00\x00\x00\x00";
	int len = 0, x, y, pos = 0;

	wxInt32 iconID;

	len = sizeof(wxUint32) + 256 + 32;
	payload = new wxUint8[len];

	/* make icon data */
	for(y = 0; y < 16; y++) 
	{
		for(x = 0; x < 16; x++) 
		{
			//m_mainWindow->logWindow->logText(wxString::Format(wxT("Red: %02X\nGreen: %02x\nBlue: %02X\n"), testImg->GetRed(x, y), testImg->GetBlue(x,y), testImg->GetGreen(x, y))); 
			index = plt->GetPixel(testImg->GetRed(x, y), testImg->GetBlue(x,y), testImg->GetGreen(x, y));
			index1 = index;
			memcpy(iconData+pos, (void*)&index1, 1);
			pos++;
		}
	}

	iconID = makeChecksum(iconData);


	memcpy(payload, &iconID, sizeof(wxUint32));
	memcpy(payload+sizeof(wxUint32), iconData, 256);
	memcpy(payload+sizeof(wxUint32)+256, padding, 32);

	GRApplication::getInstance()->getMainConnection()->sendGRPacket(CHANGE_ICON, len, payload);	
	

	delete[] payload;
	delete[] iconData;	

	this->Close();
}
//---------------------------------------------------------------------------------------
int GRChangeIconWindow::findColorIndex(unsigned char red, unsigned char green, unsigned char blue)
{
	int x;
	wxUint8 *colorTable = GRPluginManager::getInstance()->getColorTable();
	if(colorTable == NULL) {
		GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Fatal Error: Error finding color index: Color table does not exist."));
		return 0;
	}

	for(x = 0; x < 768; x++)
	{
		if(colorTable[x] == red && colorTable[x+1] == green && colorTable[x+2] == blue)
		{
			return x;
		}
	}
	return 0;
}
//------------------------------------------------------------------------------
wxPalette *GRChangeIconWindow::getMacPalette()
{
	unsigned char r[256];
	unsigned char g[256];
	unsigned char b[256];
	int i;
	wxUint8 *colorTable = GRPluginManager::getInstance()->getColorTable();
	if(colorTable == NULL) {
		GRLogger::getInstance()->log(GRLogger::LOG_ERROR, wxT("Fatal Error: Error getting mac palette: Color table does not exist."));
		return new wxPalette(256, 0, 0, 0);
	}

    for (i = 0; i < 256; i++)
    {
		r[i] = colorTable[3*i + 0];
		g[i] = colorTable[3*i + 1];
		b[i] = colorTable[3*i + 2];
    }
    return new wxPalette(256, r, g, b);
	
}
//-------------------------------------------------------------------------------
wxInt32 GRChangeIconWindow::makeChecksum(wxUint8 *iconData)
{
	int i;
	wxInt32 iconID = 0;
	wxUint32 temp;
	wxUint8 startByte[1] = {0xFF};

    for (i = 0; i < 256; i++)
            iconID += (unsigned int)(iconData[i]);

	iconID <<= 8;

	temp = iconData[0] + iconData[32] + iconData[64] + iconData[128] + iconData[255];
	temp <<= 18;
	iconID += temp;

	memcpy(&iconID, startByte, 1);
	return iconID;
}
//--------------------------------------------------------------------------------
