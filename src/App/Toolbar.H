// Toolbar.H

#ifndef TOOLBAR_H

#define TOOLBAR_H

#include <QGraphicsObject>
#include <QGraphicsView>
#include <Qt>
#include <QMap>

class Toolbar: public QGraphicsObject {
  Q_OBJECT;
public:
  Toolbar(QGraphicsView *view);
  ~Toolbar();
  Qt::Orientation orientation() const;
  void setOrientation(Qt::Orientation);
  void addTool(QString id, class ToolItem *item);
  QString selection() const;
  void select(QString);
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF boundingRect() const;
signals:
  void selectionChanged(QString id);
  void rightClick(QString id);
protected:
  virtual void doLeftClick(QString id);
  virtual void doRightClick(QString id);
private slots:
  void leftClicked();
  void rightClicked();
  void childGone();
private:
  void arrangeTools();
private:
  QGraphicsView *view;
  Qt::Orientation orient;
  QStringList ids;
  QMap<QString, ToolItem *> tools;
  QMap<ToolItem *, QString> revmap;
  QString sel;
};

#endif
