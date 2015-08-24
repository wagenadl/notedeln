// Scenes/FrontScene.H - This file is part of eln

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

// FrontScene.H

#ifndef FRONTSCENE_H

#define FRONTSCENE_H

#include <QGraphicsScene>

class FrontScene: public QGraphicsScene {
  Q_OBJECT;
public:
  FrontScene(class Notebook *book, QObject *parent=0);
  virtual ~FrontScene();
  void print(class QPrinter *, class QPainter *);
  class DefaultingQTI *addDefaultingText(QString dflt, QFont f, QColor c);
public slots:
  void rebuild();
private:
  void makeBackground();
  void makeItems();
  void positionItems();
  void makeWritable();
  void rebuildOItems();
  void recolorItems();
private slots:
  void textChange();
private:
  Notebook *book; // we do not own
  class Style const &style;
  class RoundedRect *toprect;
  class RoundedRect *bottomrect;
  class DefaultingQTI *title;
  class DefaultingQTI *author;
  class DefaultingQTI *address;
  QGraphicsTextItem *dates;
  QGraphicsTextItem *otitle;
  QGraphicsTextItem *oauthor;
  QGraphicsTextItem *oaddress;
  class QGraphicsPixmapItem *bg;
};

#endif
