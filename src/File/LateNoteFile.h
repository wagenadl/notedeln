// LateNoteFile.h

#ifndef LATENOTEFILE_H

#define LATENOTEFILE_H


#include "DataFile.h"
#include "LateNoteData.h"
#include <QDir>

typedef DataFile<LateNoteData> LateNoteFile;
LateNoteFile *createLateNoteFile(QDir const &dir, QObject *parent=0);
/* returns NULL if the file cannot be created */
LateNoteFile *loadLateNoteFile(QDir const &dir, QString uuid, QObject *parent=0);
/* returns NULL if the file cannot be found */

bool deleteLateNoteFile(QDir dir, QString uuid);

#endif
