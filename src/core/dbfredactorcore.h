/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* QDBFRedactor is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* QDBFRedactor is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		QDBFRedactor
* Author:		PanteR
* Contact:		panter.dsd@gmail.com
*******************************************************************/

#ifndef DBFREDACTORCORE_H
#define DBFREDACTORCORE_H

#ifdef unix
#include <stdint.h>
#endif //unix

namespace DBFRedactorCore {

#ifdef unix
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
#endif //unix

#ifdef WIN32
typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
#endif //WIN32
}

#endif //DBFREDACTORCORE_H
