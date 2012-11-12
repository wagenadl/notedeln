// FrontScene.H

#ifndef FRONTSCENE_H

#define FRONTSCENE_H

#include <QGraphicsScene>

class FrontScene: public QGraphicsScene {
  Q_OBJECT;
public:
  FrontScene(class Notebook *book, QObject *parent=0);
  virtual ~FrontScene();
public slots:
  void rebuild();
private:
  void makeBackground();
  void makeItems();
  void positionItems();
private:
  Notebook *book; // we do not own
  class Style const &style;
  QGraphicsTextItem *title;
  QGraphicsTextItem *author;
  QGraphicsTextItem *address;
  QGraphicsTextItem *dates;
  class QGraphicsPixmapItem *bg;
};

#endif
