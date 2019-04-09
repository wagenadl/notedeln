// App/Toolbar.H - This file is part of eln

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

// Toolbar.H

#ifndef TOOLBAR_H

#define TOOLBAR_H

#include <QGraphicsObject>
#include <Qt>
#include <QMap>

class Toolbar: public QGraphicsObject {
  Q_OBJECT;
public:
  Toolbar(QGraphicsItem *parent);
  virtual ~Toolbar();
  void enableSelect(bool t=true);
  void disableSelect();
  /* Even if selecting is disabled, clicking a button still causes the
     selectionChanged() signal to be emitted.
     The only difference is that the button is deselected upon mouse
     release.
  */
  bool isSelectEnabled() const;
  Qt::Orientation orientation() const;
  void setOrientation(Qt::Orientation);
  void addTool(QString id, class ToolItem *item);
  void addSpace(double dy);
  QString selection() const;
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF boundingRect() const;
signals:
  void selectionChanged(QString id);
  void rightClick(QString id);
public slots:
  void select(QString);
protected:
  virtual void doLeftClick(QString id, Qt::KeyboardModifiers);
  virtual void doRightClick(QString id, Qt::KeyboardModifiers);
  void timerEvent(QTimerEvent *);
private slots:
  void leftClicked(Qt::KeyboardModifiers);
  void doubleClicked(Qt::KeyboardModifiers);
  void rightClicked(Qt::KeyboardModifiers);
  void released();
  void childGone();
private:
  void arrangeTools();
private:
  Qt::Orientation orient;
  QStringList ids;
  QList<double> spaceAfter;
  QMap<QString, ToolItem *> tools;
  QMap<ToolItem *, QString> revmap;
  QString sel;
  bool selEna;
};

#endif
