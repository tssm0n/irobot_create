/***************************************************************************
 *   Copyright (C) 2007 by Kevin Shaw   *
 *   tss@cornbread.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MAPPINGCONTROLPROGRAM_H
#define MAPPINGCONTROLPROGRAM_H

#include "controlprogram.h"


#define DEFAULT_GRID_X 100
#define DEFAULT_GRID_Y 100

	// In millimeters
#define SQUARE_WIDTH 250 

enum {GRID_UNKNOWN, GRID_EMPTY, GRID_OCCUPIED};

/**
	@author Kevin Shaw <tss@cornbread.com>
*/

class MapGridObject
{
public:
	short contents;
};

class MappingControlProgram : public ControlProgram
{
public:
    MappingControlProgram();

    ~MappingControlProgram();

    void runProgram(IrobotSerial* irobotSerial);

private:
	void initializeGrid();	
	void createNewSquare(int x, int y);

	MapGridObject*** grid;
	int xSize;
	int ySize;
	int xPos;
	int yPos;

	int xPosInCell;
	int yPosInCell;

};



#endif
