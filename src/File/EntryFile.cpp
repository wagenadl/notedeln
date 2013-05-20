// EntryFile.C

#include "EntryFile.H"
#include "ResManager.H"
#include <QDebug>
#include "Assert.H"

EntryFile *createPage(QDir const &dir, int n, QObject *parent) {
  QString pfn = dir.absoluteFilePath(QString::number(n) + ".json");
  EntryFile *f = EntryFile::create(pfn, parent);
  ASSERT(f);
  ResManager *r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(QString::number(n) + ".res");
  qDebug()<< "EntryFile createPage " << pfn << resfn;
  r->setRoot(resfn);
  qDebug()<< "EntryFile createPage " << pfn << resfn;
  return f;
}

EntryFile *loadPage(QDir const &dir, int n, QObject *parent) {
  QString pfn = dir.absoluteFilePath(QString::number(n) + ".json");
  EntryFile *f = EntryFile::load(pfn, parent);
  ASSERT(f);
  ResManager *r = f->data()->firstChild<ResManager>();
  if (!r)
    r = new ResManager(f->data());
  QString resfn = dir.absoluteFilePath(QString::number(n) + ".res");
  r->setRoot(resfn);
  qDebug()<< "EntryFile loadPage " << pfn << resfn;
  return f;
}
