// Scenes/SearchResultScene.h - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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
public:
  QList<SearchResult> const &searchResults() const;
  static QMap< QString, QList<SearchResult> > allOpenSearches();
protected:
  virtual QString pgNoToString(int) const;
private:
  Style const &style() const;
  void createContinuationItem(int isheet, double ytop, double ybot);
private:
  Notebook *book;
  QString phrase;
  QString ttl;
  QList<SearchResult> results;
  QList<class SearchResItem *> headers; // one for each entry with a result
  QList<int> sheetnos; // one for each header; sheet in this scene
  static QSet<SearchResultScene const *> &allInstances();
};

#endif
