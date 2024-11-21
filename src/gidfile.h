/******************************************************************************
 *
 * This file is part of Gid-Sync.
 * Copyright (C) 2024 Gideon van der Kolf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

/* GidFile
 *
 * Safe writing of file to prevent data loss during crashes or full disks.
 *
 * G. van der Kolf, September 2024
 *
 * Provides a safe write function that first writes to a temporary file, renames
 * the original file to a backup file, and finally renames the temporary file to
 * the original file. If an error occurs at any time (i.e. computer crash or
 * disk full), the original file will remain in tact, either at its original
 * file name or as the backup file. Errors should not result in the original
 * file containing partially written new data.
 *
 * A read function is also provided that attempts to read the specified file and
 * falls back to the backup file if the specified file does not exist.
 *
 * This basically does what QSaveFile is supposed to do. However, a bug has been
 * found in QSaveFile where it could result in data loss of the original file if
 * the disk is full.
 * See: https://bugreports.qt.io/browse/QTBUG-75077
 */

#ifndef GIDFILE_H
#define GIDFILE_H

#include <QByteArray>
#include <QString>

class GidFile
{
public:
    // Suffix added to the original file name to create the temporary file
    static const QString newSuffix;
    // Suffix added to the original file name to create the backup file
    static const QString oldSuffix;

    struct Result {
        bool success = false;
        QString errorString;
    };

    struct ReadResult {
        Result result;
        QByteArray data;
    };

    static Result write(QString filename, QByteArray data);
    static ReadResult read(QString filename);
};

#endif // GIDFILE_H
