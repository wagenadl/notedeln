// SearchView.H

#ifndef SEARCHVIEW_H

#define SEARCHVIEW_H

#include <QGraphicsView>

class SearchView: public QGraphicsView {
public:
  SearchView(class SearchResultScene *scene, QWidget *parent=0);
  /* We become the owner of the scene! */
  virtual ~SearchView();
protected:
  void resizeEvent(QResizeEvent *);
  void keyPressEvent(QKeyEvent *);
  void wheelEvent(QWheelEvent *);
private:
  void gotoSheet(int n);
private:
  SearchResultScene *scene;
  double wheelDeltaAccum;
  double wheelDeltaStepSize;
  int currentSheet;
};

#endif
