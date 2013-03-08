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
#include "mappingcontrolprogram.h"

MappingControlProgram::MappingControlProgram()
 : ControlProgram()
{
	initializeGrid();
	xPos = 0;
	yPos = 0;

	xPosInCell = 0;
	yPosInCell = 0;
}


MappingControlProgram::~MappingControlProgram()
{
	for(int i = 0; i < xSize; i++){
		for(int j = 0; j < ySize; j++){
			if(grid[i][j] != NULL){
				delete grid[i][j];
			}
		}
		delete grid[i];
	}
	delete grid;
}

void MappingControlProgram::initializeGrid()
{
	grid = new MapGridObject**[DEFAULT_GRID_X];
	for(int i = 0; i < DEFAULT_GRID_X; i++){
		grid[i] = new MapGridObject*[DEFAULT_GRID_Y];
		for(int j = 0; j < DEFAULT_GRID_Y; j++){
			grid[i][j] = NULL;
		}
	}

	xSize = DEFAULT_GRID_X;
	ySize = DEFAULT_GRID_Y;

}


