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

#pragma once

#include <cstdint>

#include "time.hpp"

constexpr uint16_t TypeMask = 0170000;
constexpr uint16_t TypeSOCK = 0140000;
constexpr uint16_t TypeLNK  = 0120000;
constexpr uint16_t TypeREG  = 0100000;
constexpr uint16_t TypeBLK  = 0060000;
constexpr uint16_t TypeDIR  = 0040000;
constexpr uint16_t TypeCHR  = 0020000;
constexpr uint16_t TypeFIFO = 0010000;

constexpr uint16_t ModeMask = 07777;
constexpr uint16_t ModeSUID = 04000;
constexpr uint16_t ModeSGID = 02000;
constexpr uint16_t ModeSVTX = 01000;
constexpr uint16_t ModeRWXU = 00700;
constexpr uint16_t ModeRUSR = 00400;
constexpr uint16_t ModeWUSR = 00200;
constexpr uint16_t ModeXUSR = 00100;
constexpr uint16_t ModeRWXG = 00070;
constexpr uint16_t ModeRGRP = 00040;
constexpr uint16_t ModeWGRP = 00020;
constexpr uint16_t ModeXGRP = 00010;
constexpr uint16_t ModeRWXO = 00007;
constexpr uint16_t ModeROTH = 00004;
constexpr uint16_t ModeWOTH = 00002;
constexpr uint16_t ModeXOTH = 00001;

// An inode. This is basically the same as struct stat, but with explicit
// size of structure members.
class Inode
{
public:
    Inode();

    // functions to create specific inode types
    static Inode empty();
    static Inode directory(const Inode* parent, uint16_t mode);
    static Inode node(uint16_t typeAndMode, uint64_t rdev);
    static Inode symlink(size_t targetLen, uint64_t blockIndex);

    // data fields (see man 3type stat)
    Time atime;
    Time ctime;
    Time mtime;
    uint32_t uid;
    uint32_t gid;
    uint16_t nlink;
    uint16_t typeAndMode;
    uint64_t rdev;
    uint64_t size;
    uint64_t slotTrees[5];      // direct and indirect slots access
    uint64_t xattrIndex;        // TODO for xattr support: index of block that stores them, or InvalidIndex

    uint16_t type() const { return typeAndMode & TypeMask; }
} __attribute__((packed));
