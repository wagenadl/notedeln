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
  if (!dir.exists())
    dir.mkdir("");
  Q_ASSERT(dir.exists());
  
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

static QString resName( int i) {
  return QString("res-%1").arg(i);
}

QString ResourceManager::newName() const {
  int i=0;
  while (exists(resName(i+100)))
    i+=100;
  while (exists(resName(i+10)))
    i+=10;
  while (exists(resName(i+1)))
    i+=1;
  return resName(i+1);
}

static QString resName(QString fn, int i) {
  int idx = fn.lastIndexOf(".");
  if (idx>=0)
    return QString("%1-%2.%3").arg(fn.left(idx)).arg(i).arg(fn.mid(idx+1));
  else
    return QString("%1-%2").arg(fn).arg(i);
}

QString ResourceManager::newName(QUrl src) const {
  QStringList bits = src.path().split("/");
  if (bits.isEmpty())
    return newName();

  QString s = bits.last().replace(QRegExp("[^-A-Za-z0-9_.]+"), "");
  
  int i=0;
  while (exists(resName(s, i+100)))
    i+=100;
  while (exists(resName(s, i+10)))
    i+=10;
  while (exists(resName(s, i+1)))
    i+=1;
  return resName(s, i+1);
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

QString ResourceManager::import(QImage img, QUrl *source) {
  QString name = newName();
  img.save(path(name));
  sources[name] = source ? *source : QUrl();
  saveSources();
  return name;
}

QString ResourceManager::import(QUrl source) {
  QString name = newName(source);
  ResourceLoader *l = new ResourceLoader(source, path(name), this);
  if (l->getNowDialog()) {
    // got it
    sources[name] = source;
    saveSources();
    delete l;
    return name;
  }
  dir.remove(name);
  delete l;
  return "";
}

QString ResourceManager::link(QUrl source) {
  QString name = newName(source);
  sources[name] = source;
  saveSources();
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
      sources.remove(name);
      saveSources();
    }
    loaders[name]->deleteLater();
    loaders.remove(name);
  } else {
    qDebug() << "ResourceManager: download complete for unknown " << name;
  }
}

void ResourceManager::saveSources() const {
  QVariantMap vm;
  foreach (QString k, sources.keys()) 
    vm[k] = QVariant(sources[k].toString());
  if (!JSONFile::save(vm, dir.filePath("sources.json")))
    qDebug() << "ResourceManager::save not ok";
}

