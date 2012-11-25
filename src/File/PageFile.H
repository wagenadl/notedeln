// PageFile.H

#ifndef PAGEFILE_H

#define PAGEFILE_H

#include "DataFile.H"
#include "PageData.H"
#include <QDir>

typedef DataFile<PageData> PageFile;
PageFile *createPage(QDir const &dir, int n, QObject *parent=0);
PageFile *loadPage(QDir const &dir, int n, QObject *parent=0);

#endif
