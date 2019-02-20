/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
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

void loadConfigFile();		/* loads a config file (system/user) */
void saveConfigFile();		/* saves the config file to user space */

void configInit();		/* init internal structures and variables */
void configFree();		/* frees internal structures and variables */

void setRomDirectory(const char *);
const char *getRomDirectory();

void setDiskDirectory(const char *);
const char *getDiskDirectory();

void setTapeDirectory(const char *);
const char *getTapeDirectory();

void setCartDirectory(const char *);
const char *getCartDirectory();

void setSnapDirectory(const char *);
const char *getSnapDirectory();

/* these are remembered so they can be restored next time 
the emulator is started */
void setDiskPathDriveA(const char *);
void setDiskPathDriveB(const char *);
void setInsertedCartPath(const char *);
void setInsertedTapePath(const char *);
void setInsertedRomPath(int nSlot, const char *);
