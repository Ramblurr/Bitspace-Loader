/****************************************************************************************
 * Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>                             *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 3 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/
#ifndef LOADER_GLOBAL_H
#define LOADER_GLOBAL_H

#include "upload/UploadManager.h" //libbitspace

#include <Qt>

namespace Bitspace
{
enum ItemRoles
{
    StateRole = Qt::UserRole,
    ProgressRole,
    TotalBytesRole,
    ProcessedBytesRole,
    ElapsedTimeRole,
    RemainingTimeRole,
    SpeedRole
};

enum ItemStates
{
    Pending = 0,
    InProgress,
    Complete,
    Errored,

    Undefined
};

extern bitspace::UploadManager* Uploader;
}

#endif // LOADER_GLOBAL_H
