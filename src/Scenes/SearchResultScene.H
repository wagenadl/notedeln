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
  void gotoSheet(int);
signals:
  void pageNumberClicked(int);
protected:
  virtual QString pgNoToString(int) const;
  virtual void makeContdItems();
private:
  Style const &style() const;
  int refineBreak(QString s, int idx);
  int decentBreak(QString s, int first, int last);
  void fillText(class QTextDocument *doc, SearchResult const &res);
private:
  Notebook *book;
  QString ttl;
  QList<SearchResult> results;
  QList<class TOCItem *> headers; // one for each entry with a result
  QList<int> headersheetnos; // one for each header; sheet in this scene
  QList<QGraphicsItem *> items; // one for each result
  QList<int> itemsheetnos; // one for each result; sheet in this scene
  QList<QGraphicsItem *> lines; // separator lines
};

#endif
