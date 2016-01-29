// App/OneLink.h - This file is part of eln

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

// OneLink.h

#ifndef ONELINK_H

#define ONELINK_H

#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

class OneLink: public QObject {
  Q_OBJECT;
public:
  OneLink(class MarkupData *md, class TextItem *item);
  virtual ~OneLink();
  bool mousePress(QGraphicsSceneMouseEvent *);
  bool mouseDoubleClick(QGraphicsSceneMouseEvent *);
  void enter(QGraphicsSceneHoverEvent *);
  void leave();
  void update();
private slots:
  void downloadFinished();
public:
  QString refText() const;
  void openLink();
  void openArchive();
  bool hasArchive() const;
  bool hasPreview() const;
  class Resource *resource() const;
  void activate(QGraphicsSceneMouseEvent *);
  void contextMenu(QGraphicsSceneMouseEvent *);
  void openPage(bool newView=false);
protected:
  void getArchiveAndPreview();
private:
  MarkupData *md; // we do not own
  TextItem *ti; // we do not own
  class PreviewPopper *popper; // we own
  QString lastRef;
  bool lastRefIsNew;
  bool busy;  
};

#endif
