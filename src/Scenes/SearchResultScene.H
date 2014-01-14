// SearchResultScene.H

#ifndef SEARCHRESULTSCENE_H

#define SEARCHRESULTSCENE_H

#include "BaseScene.H"
#include "Search.H"

class SearchResultScene: public BaseScene {
  Q_OBJECT;
public:
  SearchResultScene(QString phrase, QString title, QList<SearchResult> results,
                    Data *data, QObject *parent=0);
  virtual ~SearchResultScene();
  void update(QList<SearchResult> results);
  virtual void populate();
  virtual QString title() const;
public slots:
  void gotoSheet(int);
  void pageNumberClick(int);
signals:
  void pageNumberClicked(int, QString);
protected:
  virtual QString pgNoToString(int) const;
  virtual void makeContdItems();
private:
  Style const &style() const;
private:
  Notebook *book;
  QString phrase;
  QString ttl;
  QList<SearchResult> results;
  QList<class SearchResItem *> headers; // one for each entry with a result
  QList<int> sheetnos; // one for each header; sheet in this scene
};

#endif
