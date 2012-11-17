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
#include <QProcess>

ResourceManager::ResourceManager(QString resdir, QObject *parent):
  QObject(parent), dir(resdir) {
  if (!dir.exists())
    QDir().mkpath(dir.absolutePath());
  Q_ASSERT(dir.exists());
  
  mapper = new QSignalMapper(this);
  connect(mapper, SIGNAL(mapped(QString)), SLOT(downloadComplete(QString)));
  procMapper = new QSignalMapper(this);
  connect(procMapper, SIGNAL(mapped(QString)), SLOT(processComplete(QString)));
  
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

static QString mkName(int i) {
  return QString("res-%1").arg(i);
}

QString ResourceManager::newName() const {
  int i=0;
  while (exists(mkName(i+100)))
    i+=100;
  while (exists(mkName(i+10)))
    i+=10;
  while (exists(mkName(i+1)))
    i+=1;
  return mkName(i+1);
}

static QString mkName(QString fn, int i) {
  int idx = fn.lastIndexOf(".");
  if (idx>=0)
    return QString("%1-%2.%3").arg(fn.left(idx)).arg(i).arg(fn.mid(idx+1));
  else
    return QString("%1-%2").arg(fn).arg(i);
}

QString ResourceManager::newName(QUrl src) const {
  QString s = src.host();
  if (s.isEmpty()) {
    QStringList bits = src.path().split("/");
    if (bits.isEmpty())
      return newName();
    s = bits.last();
  } else {
    s += ".";
  }
  s = s.replace(QRegExp("[^-A-Za-z0-9_.]+"), "");
  
  int i=0;
  while (exists(mkName(s, i+100)))
    i+=100;
  while (exists(mkName(s, i+10)))
    i+=10;
  while (exists(mkName(s, i+1)))
    i+=1;
  return mkName(s, i+1);
}

QString ResourceManager::path(QString name) const {
  return dir.absoluteFilePath(name);
}

bool ResourceManager::complete(QString name) const {
  if (!exists(name))
    return false;
  if (loaders.contains(name))
    return false;
  if (processes.contains(name))
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

QString ResourceManager::resName(QUrl src) const {
  for (QMap<QString, QUrl>::const_iterator i=sources.begin();
       i!=sources.end(); ++i)
    if (i.value()==src)
      return i.key();
  return "";
}

QString ResourceManager::import(QImage img, QUrl const *source) {
  if (source) {
    QString name = resName(*source);
    if (!name.isEmpty())
      return name; // don't reimportant
  }
  QString name = newName(QUrl("import.png")); // is that good default?
  qDebug() << "ResourceManager::import" << name << path(name) << source;
  Q_ASSERT(img.save(path(name)));
  sources[name] = source ? *source : QUrl();
  saveSources();
  return name;
}

QString ResourceManager::import(QUrl source) {
  QString name = resName(source);
  if (!name.isEmpty())
    return name; // don't reimportant

  name = newName(source);
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
  QString name = resName(source);
  qDebug() << "ResMgr" << source << name;
  if (!name.isEmpty())
    return name;
      
  name = newName(source);
  sources[name] = source;
  saveSources();
  loaders[name] = new ResourceLoader(source, path(name), this);
  connect(loaders[name], SIGNAL(finished()),
	  mapper, SLOT(map()));
  mapper->setMapping(loaders[name], name);
  return name;
}

void ResourceManager::downloadComplete(QString name) {
  qDebug() << "downloadComplete" << name;
  if (loaders.contains(name)) {
    if (loaders[name]->failed()) {
      qDebug() << "download failed";
      QFile res(path(name));
      res.remove();
      sources.remove(name);
      saveSources();
    } else {
      QString mime = loaders[name]->mimeType();
      qDebug() << "ResourceManager: download" << name << "has type" <<mime;
      // if mime is html, we will convert to pdf
      if (mime.startsWith("text/html")) {
	makePdfAndThumb(name); 
      }
    }
    loaders[name]->deleteLater();
    loaders.remove(name);
  } else {
    qDebug() << "ResourceManager: download complete for unknown " << name;
  }
}

void ResourceManager::processComplete(QString name) {
  qDebug() << "ResourceManager: process complete " << name;
  QProcess *p = processes[name];
  Q_ASSERT(p);
  if (p->exitStatus()!=QProcess::NormalExit
      || p->exitCode()!=0) {
    // that didn't work
    qDebug() << "  failed";
    QFile pdf(path(name) + "pdf");
    pdf.remove();
    QFile png(path(name) + "png");
    png.remove();
    sources.remove(name);
    saveSources();
  } else {
    qDebug() << "  success";
    // that did work
  }
  processes[name]->deleteLater();
  processes.remove(name);
}

void ResourceManager::makePdfAndThumb(QString name) {
  QUrl src = sources[name];
  processes[name] = new QProcess(this);
  QStringList args;
  args.append(src.toString());
  args.append(path(name)+"pdf");
  args.append(path(name)+"png");
  connect(processes[name], SIGNAL(finished(int, QProcess::ExitStatus )),
	  procMapper, SLOT(map())); // for some reason this doesn't work
  mapper->setMapping(processes[name], name);
  qDebug() << "starting webgrab";
  // must deal with stdio from process?
  x
  processes[name]->start("webgrab", args);
}

void ResourceManager::saveSources() const {
  QVariantMap vm;
  foreach (QString k, sources.keys()) 
    vm[k] = QVariant(sources[k].toString());
  if (!JSONFile::save(vm, dir.filePath("sources.json")))
    qDebug() << "ResourceManager::save not ok";
}

