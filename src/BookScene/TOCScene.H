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
  void rebuild();
  virtual void gotoSheet(int);
protected:
  virtual QString title() const;
  virtual QString pgNoToString(int) const;
private:
  TOC *data;
  QList<class TOCItem *> items;
  QList<int> sheetNos;
};

#endif
