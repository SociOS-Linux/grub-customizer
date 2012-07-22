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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "md5.h"

std::string md5(std::string const& input) {
	unsigned char cresult[16];

	unsigned char buf[16];
	unsigned char* cStr = new unsigned char[input.length() + 1];
	for (int i = 0; i < input.length(); i++) {
		cStr[i] = static_cast<unsigned char>(input[i]);
	}
	MD5(cStr, input.length(), buf);

	std::string result;
	for (int i = 0; i < 16; i++) {
		unsigned int a = (buf[i] - (buf[i] % 16)) / 16;
		if (a <= 9) {
			result += '0' + a;
		} else {
			result += 'a' + a - 10;
		}
		unsigned int b = (buf[i] % 16);
		if (b <= 9) {
			result += '0' + b;
		} else {
			result += 'a' + b - 10;
		}
	}
	return result;
}