// SearchResultScene.H

#ifndef SEARCHRESULTSCENE_H

#define SEARCHRESULTSCENE_H

#include "BaseScene.h"
#include "Search.h"

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
  void pageNumberClick(int, Qt::KeyboardModifiers, QString); // pgno, uuid
signals:
  void pageNumberClicked(int, Qt::KeyboardModifiers,
                         QString, QString); // pgno, uuid, phrase
protected:
  virtual QString pgNoToString(int) const;
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
