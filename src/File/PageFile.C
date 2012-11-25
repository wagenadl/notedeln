// PageFile.C

#include "PageFile.H"
#include "Resources.H"
#include <QDebug>

PageFile *createPage(QDir const &dir, int n, QObject *parent) {
  QString pfn = dir.absoluteFilePath(QString::number(n) + ".json");
  PageFile *f = PageFile::create(pfn, parent);
  Resources *r = new Resources(f->data());
  QString resfn = dir.absoluteFilePath(QString::number(n) + ".res");
  qDebug()<< "PageFile createPage " << pfn << resfn;
  r->setRoot(resfn);
  qDebug()<< "PageFile createPage " << pfn << resfn;
  return f;
}

PageFile *loadPage(QDir const &dir, int n, QObject *parent) {
  QString pfn = dir.absoluteFilePath(QString::number(n) + ".json");
  PageFile *f = PageFile::load(pfn, parent);
  Resources *r = f->data()->firstChild<Resources>();
  if (!r)
    r = new Resources(f->data());
  QString resfn = dir.absoluteFilePath(QString::number(n) + ".res");
  r->setRoot(resfn);
  qDebug()<< "PageFile loadPage " << pfn << resfn;
  return f;
}
