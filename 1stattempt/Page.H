// Page.H

#ifndef PAGE_H

#define PAGE_H

#include <QObject>

/* Page is a single logical page in the notebook. A Page has a fixed width
   as set in Style, but it does not have a fixed height. Splitting over
   multiple physical pages is a desirable feature, but will not be implemented
   in the first instance. For now, a Page will contain a single GraphicsScene
   that contains all the items on the page. Later, I think I will make a Page
   contain several GraphicsScenes to enable proper pagination.
 */

class Page: public QObject {
  Q_OBJECT;
public:
  Page(QObject *parent=0);
  virtual ~Page();
  void attach(QString filename);
  void create(QString filename);
  PageChunk *currentChunk();
  QList<PageChunk const *> chunks() const;
public:
  TitleHistory const *titles() const;
  void setTitle(QString title);
private slots:
  void writeXML();
private:
  QString filename;
  QDomDocument xml;
  TitleHistory *titleHistory;
  QList<PageChunk *> contents;
};

#endif
