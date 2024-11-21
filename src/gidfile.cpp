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

#include "gidfile.h"

#include <QTemporaryFile>

const QString GidFile::newSuffix("~$new");
const QString GidFile::oldSuffix("~$old");


GidFile::Result GidFile::write(QString filename, QByteArray data)
{
    Result ret;

    // Create temporary file
    QTemporaryFile ftemp;
    ftemp.setAutoRemove(false);
    ftemp.setFileTemplate(filename + newSuffix);
    if (!ftemp.open()) {
        ret.success = false;
        ret.errorString = "Failed to open temporary file: " + ftemp.errorString();
        return ret;
    }

    // Write to temporary file
    qint64 nwritten = ftemp.write(data);
    if (nwritten != data.count()) {
        ret.success = false;
        ret.errorString = QString("Only wrote %1 of %2 bytes: %3")
                .arg(nwritten).arg(data.count()).arg(ftemp.errorString());
        return ret;
    }

    // Flush and close temporary file
    if (!ftemp.flush()) {
        ret.success = false;
        ret.errorString = "Failed to flush temporary file: " + ftemp.errorString();
        return ret;
    }
    ftemp.close();
    // Now temporary file should be all good, waiting to be renamed to original file

    // Backup original file
    QFile forig(filename);
    if (forig.exists()) {
        QFile fbackup(filename + oldSuffix);
        if (fbackup.exists()) {
            // First remove old backup file
            if (!fbackup.remove()) {
                ret.success = false;
                ret.errorString = "Failed to remove previous backup file: "
                                  + fbackup.errorString();
                return ret;
            }
        }
        // Rename original file to backup file
        if (!forig.rename(fbackup.fileName())) {
            ret.success = false;
            ret.errorString = "Failed to backup original file: "
                              + forig.errorString();
            return ret;
        }
    }

    // Rename temporary file, overwriting original file

    // Try to rename atomically (i.e. rename replaces original file)
    if (!ftemp.rename(filename)) {
        // Atomic rename failed. Delete original file first, then retry rename.
        QFile forig(filename);
        if (forig.exists()) {
            if (!forig.remove()) {
                // Do not fail yet.
            }
        }
        if (!ftemp.rename(filename)) {
            ret.success = false;
            ret.errorString = "Failed to replace original file: "
                              + ftemp.errorString();
            return ret;
        }
    }

    ret.success = true;
    return ret;
}

GidFile::ReadResult GidFile::read(QString filename)
{
    ReadResult ret;

    QFile f(filename);
    if (!f.exists()) {
        // File does not exist. Try the backup file.
        f.setFileName(f.fileName() + oldSuffix);
    }
    if (!f.open(QIODevice::ReadOnly)) {
        ret.result.success = false;
        ret.result.errorString = "Failed to open file: " + f.errorString();
        return ret;
    }

    ret.data = f.readAll();
    f.close();

    ret.result.success = true;
    return ret;
}
