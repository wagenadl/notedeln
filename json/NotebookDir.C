// NotebookDir.C

#include "NotebookDir.H"

NotebookDir::NotebookDir(QString path) {
  root = QDir(path);
}

NotebookDir *NotebookDir::load(QString path) {
  QDir d(path);
  if (d.exists())
    return new NotebookDir(d.absolutePath());
  else
    return 0;
}

NotebookDir *NotebookDir::create(QString path) {
  QDir d(path);
  if (d.exists()) 
    return 0;

  if (!d.mkpath("pages")) 
    return 0;
  
  if (!d.mkpath("res"))
    return 0;

  NotebookDir *nb = new NotebookDir(d.absolutePath());
  delete TOCFile::create(nb->tocPath()); // construct an empty TOC
}

QString NotebookDir::tocPath() const {
  return root.absoluteFilePath("toc.json");
}

QString NotebookDir::pagePath(int page) const {
  return root.absoluteFilePath(QString("pages/%1.json")
			       .arg(page, 3, 10, QChar('0')));  
}

QString NotebookDir::resDir(int) const {
  return root.absoluteFilePath("res");
}
  
QString NotebookDir::resPath(QString name) const {
  QDir res = root;
  res.cd("res");
  return res.absoluteFilePath(name);
}

bool NotebookDir::resExists(QString name) const {
  QFile f(resPath(name));
  return f.exists();
}

static QString resName(int pgno, int i) {
  return QString("%1-%1.png")
    .arg(pgno, 3, 10, QChar('0'))
    .arg(i, 3, 10, QChar('0'));
}

QString NotebookDir::newResource(QImage source, int destPage) const {
  int i = 0;
  while (QFile(resPath(resName(destPage, i+100)).exists()))
    i+=100;
  while (QFile(resPath(resName(destPage, i+10)).exists()))
    i+=10;
  while (QFile((resPath(resName(destPage, i+1)).exists())))
    i+=1;
  QString fn = resName(destPage, i+1);
  if (source.save(resPath(fn)))
    return fn;
  else
    return "";
}

QString NotebookDir::newResource(QUrl source, int destPage) const {
  int i = 0;
  while (QFile(resPath(resName(destPage, i+100)).exists()))
    i+=100;
  while (QFile(resPath(resName(destPage, i+10)).exists()))
    i+=10;
  while (QFile((resPath(resName(destPage, i+1)).exists())))
    i+=1;
  QString fn = resName(destPage, i+1);
  
