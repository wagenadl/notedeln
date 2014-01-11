// SearchResultScene.H

#ifndef SEARCHRESULTSCENE_H

#define SEARCHRESULTSCENE_H

#include "BaseScene.H"
#include "Search.H"

class SearchResultScene: public BaseScene {
  Q_OBJECT;
public:
  SearchResultScene(QString title, QList<SearchResult> results,
                    Data *data, QObject *parent=0);
  virtual ~SearchResultScene();
  void update(QList<SearchResult> results);
  virtual void populate();
  virtual QString title() const;
public slots:
  void gotoSheet();
signals:
  void pageNumberClicked(int);
private:
  QString ttl;
  QList<SearchResult> results;
  QList<class TOCEntry *> mocktoc;
  QList<QGraphicsItem *> items; // one for each result
  QList<int> sheetnos; // one for each result
  QList<QGraphicsItem *> lines;
};

#endif
