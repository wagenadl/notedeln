// VideoSlider.h

#ifndef VIDEOSLIDER_H

#define VIDEOSLIDER_H

#include <QGraphicsObject>

class VideoSlider: public QGraphicsObject {
  Q_OBJECT;
public:
  VideoSlider();
  virtual ~VideoSlider();
  void setDuration(double t_s);
  void setPosition(double t_s);
  void resize(double width, double height);
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *) override;
  QRectF boundingRect() const override;
signals:
  void sliderDragged(double t_s);
private:
  void moveEllipse();
  double time2x(double t_s) const;
  double x2time(double x) const;
private:
  double dur_s;
  double pos_s;
  double width, height;
  QGraphicsLineItem *line;
  QGraphicsEllipseItem *ellipse;
};

#endif
