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
  ~Toolbar();
  Qt::Orientation orientation() const;
  void setOrientation(Qt::Orientation);
  void addTool(QString id, class ToolItem *item);
  QString selection() const;
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  virtual QRectF boundingRect() const;
signals:
  void selectionChanged(QString id);
  void rightClick(QString id);
public slots:
  void select(QString);
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
  Qt::Orientation orient;
  QStringList ids;
  QMap<QString, ToolItem *> tools;
  QMap<ToolItem *, QString> revmap;
  QString sel;
};

#endif
