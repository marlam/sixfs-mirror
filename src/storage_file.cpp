/*
 * Copyright (C) 2023, 2024
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

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include "storage_file.hpp"
#include "logger.hpp"


// flag that tells us if punching holes is support by the underlying
// filesystem so that we don't retry indefinitely
volatile int dontPunchHoles;


StorageFile::StorageFile(const std::string& name) : _name(name), _fd(0)
{
}

StorageFile::~StorageFile()
{
    close();
}

int StorageFile::open()
{
    int r = ::open(_name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (r < 0)
        return -errno;
    _fd = r;
    return 0;
}

int StorageFile::close()
{
    if (_fd != 0) {
        int r = ::close(_fd);
        _fd = 0;
        if (r < 0)
            return -errno;
    }
    return 0;
}

int StorageFile::stat(uint64_t* maxBytes, uint64_t* availableBytes)
{
    struct statvfs statfsbuf;
    if (statvfs(_name.c_str(), &statfsbuf) != 0)
        return -errno;
    *maxBytes = uint64_t(statfsbuf.f_blocks) * uint64_t(statfsbuf.f_frsize);
    *availableBytes = uint64_t(statfsbuf.f_bavail) * uint64_t(statfsbuf.f_frsize);
    return 0;
}

int StorageFile::sizeInBytes(uint64_t* s)
{
    struct stat statbuf;
    if (fstat(_fd, &statbuf) != 0)
        return -errno;
    *s = statbuf.st_size;
    return 0;
}

int StorageFile::readBytes(uint64_t index, uint64_t size, void* buf)
{
    while (size > 0) {
        ssize_t r = ::pread(_fd, buf, size, index);
        if (r < 0)
            return -errno;
        size -= r;
        index += r;
    }
    return 0;
}

int StorageFile::writeBytes(uint64_t index, uint64_t size, const void* buf)
{
    while (size > 0) {
        ssize_t r = ::pwrite(_fd, buf, size, index);
        if (r < 0)
            return -errno;
        size -= r;
        index += r;
    }
    return 0;
}

int StorageFile::punchHoleBytes(uint64_t index, uint64_t size)
{
    if (!dontPunchHoles) {
        int r = fallocate(_fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, index, size);
        if (r != 0) {
            // Not all filesystems support this, but that's ok, our structure is still valid.
            // But remember this outcome so that we don't try again needlessly.
            dontPunchHoles = 1;
            logger.log(Logger::Warning, "punching a hole failed, not trying again: %s", strerror(errno));
        }
    }
    return 0;
}

int StorageFile::setSizeBytes(uint64_t size)
{
    if (ftruncate(_fd, size) != 0)
        return -errno;
    return 0;
}
