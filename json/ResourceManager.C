// ResourceManager.C

#include "ResourceManager.H"
#include <QSignalMapper>
#include <QFile>
#include <QFileInfo>
#include "JSONFile.H"
#include <QDebug>
#include <QDateTime>
#include "ResourceLoader.H"
#include <QRegExp>

ResourceManager::ResourceManager(QString resdir, QObject *parent):
  QObject(parent), dir(resdir) {
  mapper = new QSignalMapper(this);
  connect(mapper, SIGNAL(mapped(QString)), SLOT(downloadComplete(QString)));
  
  QFile src(dir.filePath("sources.json"));
  if (src.exists()) {
    bool ok;
    QVariantMap m = JSONFile::load(src.fileName(), &ok);
    if (!ok) {
      qDebug() << "Failure to read source list";
      return;
    }
    foreach (QString k, m.keys()) 
      sources[k] = m[k].toUrl();
  }
}


bool ResourceManager::exists(QString name) const {
  QFile f(dir.filePath(name));
  return f.exists();
}

static QString resName(int pgno, int i) {
  return QString("%1-%2").arg(pgno).arg(i);
}

QString ResourceManager::newName(int pgno) const {
  int i=0;
  while (exists(resName(pgno, i+100)))
    i+=100;
  while (exists(resName(pgno, i+10)))
    i+=10;
  while (exists(resName(pgno, i+1)))
    i+=1;
  return resName(pgno, i+1);
}

static QString resName(QString fn, int pgno, int i) {
  return QString("%1-%2-%3").arg(pgno).arg(i).arg(fn);
}

QString ResourceManager::newName(QUrl src, int pgno) const {
  QStringList bits = src.path().split("/");
  if (bits.isEmpty())
    return newName(pgno);

  QString s = bits.last().replace(QRegExp("[^-A-Za-z0-9_.]+"), "");
  
  int i=0;
  while (exists(resName(s, pgno, i+100)))
    i+=100;
  while (exists(resName(s, pgno, i+10)))
    i+=10;
  while (exists(resName(s, pgno, i+1)))
    i+=1;
  return resName(s, pgno, i+1);
}

QString ResourceManager::path(QString name) const {
  return dir.absoluteFilePath(name);
}

bool ResourceManager::complete(QString name) const {
  if (!exists(name))
    return false;
  if (loaders.contains(name))
    return false;
  return true;
}

bool ResourceManager::outdated(QString name) const {
  if (!sources.contains(name))
    return false;
  if (!sources[name].isLocalFile())
    return false;
  QFileInfo f(sources[name].toLocalFile());
  if (!f.exists())
    return false;
  QFileInfo f0(path(name));
  return f.lastModified() > f0.lastModified();
}

QString ResourceManager::import(QImage img, int pgno) {
  QString name = newName(pgno);
  img.save(path(name));
  return name;
}

QString ResourceManager::import(QUrl source, int pgno) {
  QString name = newName(source, pgno);
  ResourceLoader *l = new ResourceLoader(source, path(name), this);
  if (l->getNowDialog()) {
    // got it
    sources[name] = source;
  }
  delete l;
  return name;
}

QString ResourceManager::link(QUrl source, int pgno) {
  QString name = newName(source, pgno);
  sources[name] = source;
  loaders[name] = new ResourceLoader(source, path(name), this);
  connect(loaders[name], SIGNAL(finished()),
	  mapper, SLOT(map()));
  mapper->setMapping(loaders[name], name);
  return name;
}

void ResourceManager::downloadComplete(QString name) {
  if (loaders.contains(name)) {
    if (loaders[name]->failed()) {
      QFile res(path(name));
      res.remove();
    }
    loaders[name]->deleteLater();
    loaders.remove(name);
  }
}
