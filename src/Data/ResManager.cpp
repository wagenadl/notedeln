// Data/ResManager.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// ResManager.C

#include "ResManager.h"
#include <QImage>
#include <QDebug>

static Data::Creator<ResManager> c("resources");

ResManager::ResManager(Data *parent): Data(parent) {
  setType("resources");
}

ResManager::~ResManager() {
}

Resource *ResManager::byTag(QString t) const {
  foreach (Resource *r, children<Resource>()) 
    if (r->tag()==t)
      return r;
  return 0;
}

Resource *ResManager::byURL(QUrl t) const {
  foreach (Resource *r, children<Resource>()) 
    if (r->sourceURL()==t)
      return r;
  return 0;
}

void ResManager::setRoot(QString d) {
  dir = d;
  foreach (Resource *r, children<Resource>())
    r->setRoot(d);
}

Resource *ResManager::importImage(QImage img, QUrl source) {
  Resource *res = newResource();
  res->setSourceURL(source);
  res->importImage(img);
  return res;
}

Resource *ResManager::getArchiveAndPreview(QUrl source, QString altRes) {
  Resource *res = newResource(altRes);
  res->setSourceURL(source);
  res->getArchiveAndPreview();
  return res;
}

void ResManager::perhapsDropResource(QString tag) {
  Resource *res = byTag(tag);
  if (res && isAnyoneUsing(tag))
    dropResource(res);
}

bool ResManager::isAnyoneUsing(QString tag, Data *tree) const {
  if (!tree)
    tree = parent();
  if (tree->resourceTags().contains(tag))
    return true;
  foreach (Data *d, tree->allChildren())
    if (isAnyoneUsing(tag, d))
      return true;
  return false;
}
 
void ResManager::dropResource(Resource *r) {
  if (!r)
    return;
  qDebug() << "dropresource" << r->tag() << r->sourceURL();
  QString ap = r->hasArchive() ? r->archivePath() : "";
  QString pp = r->hasPreview() ?  r->previewPath() : "";
  if (!deleteChild(r))
    qDebug() << "Dropping resource that isn't a child" << r;
  if (!ap.isEmpty()) {
    QFile a(ap);
    a.remove();
  }
  if (!pp.isEmpty()) {
    QFile a(pp);
    a.remove();
  }
}

Resource *ResManager::newResource(QString altRes) {
  Resource *res = new Resource(this);
  res->setRoot(dir.absolutePath());
  if (altRes.isEmpty()) {
    int n = 1;
    while (byTag("*" + QString::number(n))) 
      n++;
    res->setTag("*" + QString::number(n));
  } else {
    res->setTag(altRes);
  }
  return res;
}
