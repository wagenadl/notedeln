// SheetScene.H

#ifndef SHEETSCENE_H

#define SHEETSCENE_H

#include <QGraphicsScene>
#include <QDate>

class SheetScene: public QGraphicsScene {
  Q_OBJECT;
public:
  SheetScene(class Style &s, QObject *parent);
  virtual ~SheetScene();
  void setDate(QDate const &date);
  void setNOfN(int n, int N);
  void setTitle(QString const &title);
  void setFancyTitle(class TextData *data);
  void setPageNumber(int n, bool roman=false);
  void setPageNumber(QString n);
signals:
  void fancyTitleChanged();
private:
  QGraphicsTextItem *pgnoItem;
  QGraphicsTextItem *titleItem;
  class TextItem *fancyTitleItem;
  QGraphicsTextItem *contItem;
  QGraphicsTextItem *contdItem;
  QGraphicsTextItem *nOfNItem;
  QGraphicsTextItem *dateItem;
  QGraphicsLineItem *leftMarginItem;
  QGraphicsLineItem *topMarginItem;
};

#endif
