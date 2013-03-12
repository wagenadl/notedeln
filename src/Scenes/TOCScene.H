// TOCScene.H

#ifndef TOCSCENE_H

#define TOCSCENE_H

#include "BaseScene.H"

class TOCScene: public BaseScene {
  Q_OBJECT;
public:
  TOCScene(class TOC *data, QObject *parent=0);
  virtual void populate();
  virtual ~TOCScene();
public slots:
  void tocChanged();
  void itemChanged();
  virtual void gotoSheet(int);
signals:
  void pageNumberClicked(int);
protected:
  virtual QString title() const;
  virtual QString pgNoToString(int) const;
  virtual void makeContdItems();
private:
  void rebuild();
  void relayout();
private:
  TOC *data;
  QList<class TOCItem *> items;
  QList<class QGraphicsLineItem *> lines;
  QList<int> sheetNos;
};

#endif
