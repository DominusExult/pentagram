/*
 *  Copyright (C) 2004-2006  The Pentagram Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "pent_include.h"
#include "MessageBoxGump.h"

#include "TextWidget.h"
#include "ButtonWidget.h"
#include "GameData.h"
#include "Shape.h"
#include "GumpShapeArchive.h"
#include "ShapeFrame.h"
#include "UCMachine.h"
#include "GumpNotifyProcess.h"
#include "Item.h"
#include "getObject.h"
#include "CoreApp.h"
#include "GameInfo.h"
#include "util.h"
#include "FontManager.h"
#include "Font.h"
#include "GUIApp.h"
#include "RenderSurface.h"

#include "IDataSource.h"
#include "ODataSource.h"

DEFINE_RUNTIME_CLASSTYPE_CODE(MessageBoxGump,ModalGump);

MessageBoxGump::MessageBoxGump()
	: ModalGump()
{

}

MessageBoxGump::MessageBoxGump(const std::string & title_, const std::string & message_, uint32 title_colour_, 
						   std::vector<std::string> *buttons_) :
	ModalGump(0, 0, 100, 100), title(title_), message(message_), title_colour(title_colour_)
{
	if (buttons_) buttons_->swap(buttons);
	if (buttons.empty()) buttons.push_back(std::string("Ok"));
}

MessageBoxGump::~MessageBoxGump(void)
{
}

#define MBG_PADDING 16

void MessageBoxGump::InitGump(Gump* newparent, bool take_focus)
{
	ModalGump::InitGump(newparent, take_focus);

	// work out sizes of the text
	Pentagram::Font *font = FontManager::get_instance()->getTTFont(0);

	int width;
	int height;
	unsigned int rem;

	// Title width
	font->getStringSize(title,width,height);
	int title_w = width;

	// Width of buttons
	int buttons_w = MBG_PADDING;
	for (size_t i = 0; i < buttons.size(); i++)
	{
		width = height = 0;
		font->getStringSize(buttons[i],width,height);
		buttons_w += width + MBG_PADDING;
	}

	// Message size
	font->getTextSize(message,width,height,rem);
	
	dims.w = MBG_PADDING+width+MBG_PADDING;
	if (dims.w < MBG_PADDING+title_w+MBG_PADDING) dims.w = MBG_PADDING+title_w+MBG_PADDING;
	if (dims.w < buttons_w) dims.w = buttons_w;

	dims.h = 23 + MBG_PADDING + height + MBG_PADDING + 28;

	// Title 
	Gump* w = new TextWidget(MBG_PADDING, 2, title, false, 0);
	w->InitGump(this, false);

	// Message
	w = new TextWidget(MBG_PADDING, 23 + MBG_PADDING, message, false, 0, width, height);
	w->InitGump(this, false);

	// Buttons (right aligned)
	int off = dims.w-buttons_w;
	for (size_t i = 0; i < buttons.size(); i++)
	{
		w = new ButtonWidget(off, dims.h-23, buttons[i], false, 1, 0x80D000D0);
		w->SetIndex(static_cast<sint32>(i));
		w->InitGump(this, false);
		width = height = 0;
		font->getStringSize(buttons[i],width,height);
		off += width + MBG_PADDING;
	}

	GUIApp* guiapp = GUIApp::get_instance();
	guiapp->pushMouseCursor();
	guiapp->setMouseCursor(GUIApp::MOUSE_POINTER);
}

void MessageBoxGump::Close(bool no_del)
{
	GUIApp* guiapp = GUIApp::get_instance();
	guiapp->popMouseCursor();

	ModalGump::Close(no_del);
}

void MessageBoxGump::PaintThis(RenderSurface* surf, sint32 lerp_factor, bool /*scaled*/)
{
	// Background is partially transparent
	surf->FillBlended(0x80000000,0,0,dims.w,dims.h);

	uint32 line_colour = 0xFFFFFFFF;
	if (!IsFocus()) line_colour = 0xFF7F7F7F;

	// outer border
	surf->Fill32(line_colour,0,0,dims.w,1);
	surf->Fill32(line_colour,0,0,1,dims.h);
	surf->Fill32(line_colour,0,dims.h-1,dims.w,1);
	surf->Fill32(line_colour,dims.w-1,0,1,dims.h);

	// line above buttons
	surf->Fill32(line_colour,0,dims.h-28,dims.w,1);

	// line below title
	surf->Fill32(line_colour,0,23,dims.w,1);

	// Highlight behind message..
	if (IsFocus()) surf->Fill32(title_colour,1,1,dims.w-2,22);
	else surf->Fill32(0xFF000000,1,1,dims.w-2,22);
}

void MessageBoxGump::ChildNotify(Gump *child, uint32 message)
{
	if (child->IsOfType<ButtonWidget>() &&
		message == ButtonWidget::BUTTON_CLICK)
	{
		process_result = child->GetIndex();
		Close();
	}
}

ProcId MessageBoxGump::Show(std::string title, std::string message, uint32 title_colour, std::vector<std::string> *buttons)
{
	Gump *gump = new MessageBoxGump(title,message,title_colour,buttons);
	gump->InitGump(0);
	gump->setRelativePosition(CENTER);
	gump->CreateNotifier();
	return gump->GetNotifyProcess()->getPid();
}

void MessageBoxGump::saveData(ODataSource* ods)
{
	CANT_HAPPEN_MSG("Trying to load ModalGump");
}

bool MessageBoxGump::loadData(IDataSource* ids, uint32 version)
{
	CANT_HAPPEN_MSG("Trying to load ModalGump");

	return false;
}

