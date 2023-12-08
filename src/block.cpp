/*
 * Copyright (C) 2023
 * Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstring>

#include "index.hpp"
#include "block.hpp"


Block::Block()
{
}

void Block::initializeData()
{
    memset(data, 0, sizeof(data));
}

void Block::initializeIndices()
{
    for (size_t i = 0; i < (sizeof(indices) / sizeof(indices[0])); i++)
        indices[i] = InvalidIndex;
}

void Block::initializeTarget()
{
    memset(data, 0, sizeof(data));
}