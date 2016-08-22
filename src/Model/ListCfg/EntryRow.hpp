/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef GRUB_CUSTOMIZER_ENTRYROW_INCLUDED
#define GRUB_CUSTOMIZER_ENTRYROW_INCLUDED

#include <cstdio>

namespace Gc { namespace Model { namespace ListCfg { class EntryRow
{
	public: EntryRow(FILE* sourceFile) :
		eof(false),
		is_loaded(true)
	{
		this->eof = true; //will be set to false on the first loop run
		int c;
		while ((c = fgetc(sourceFile)) != EOF){
			this->eof = false;
			if (c != '\n'){
				this->text += char(c);
			}
			else {
				break;
			}
		}
	}

	public: EntryRow() :
		eof(false),
		is_loaded(true)
	{}

	public: std::string text;
	public: bool eof;
	public: bool is_loaded;
	public: operator bool()
	{
		return !eof && is_loaded;
	}
};}}}

#endif
