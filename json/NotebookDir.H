// NotebookDir.H

#ifndef NOTEBOOKDIR_H

#define NOTEBOOKDIR_H

#include <QString>
#include <QDir>
#include <QFile>

class NotebookDir: public QObject {
  Q_OBJECT;
public:
  static NotebookDir *load(QString path);
  static NotebookDir *create(QString path);
  /* Returns null if couldn't create, e.g., if already exists */
public:
  QString tocPath() const;
  QString pagePath(int page) const;
  QString resDir(int page) const;
  QString resPath(QString name) const;
  /* Returns the absolute path of the NAMEd resource, even if nonexistent */
  bool resExists(QString name) const;
  QString newResource(QUrl source, int destPage) const;
  /* Fetches the resource and returns a resource NAME or empty if not
     fetchable. */
  QString newResource(QImage source, int destPage) const;
  /* Creates a new resource from an image and returns its NAME. */
private:
  NotebookDir(QString path);
private:
  QDir root;
};

#endif
