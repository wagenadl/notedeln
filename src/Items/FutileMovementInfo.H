// FutileMovementInfo.H

#ifndef FUTILEMOVEMENTINFO_H

#define FUTILEMOVEMENTINFO_H

#include <Qt>
#include <QPointF>

class FutileMovementInfo {
public:
  FutileMovementInfo();
  FutileMovementInfo(int key, Qt::KeyboardModifiers mod, class TextItem *src);
  QPointF scenePos() const;
  int key() const;
  Qt::KeyboardModifiers modifiers() const;
private:
  QPointF scenePos_;
  int key_;
  Qt::KeyboardModifiers modifiers_;
};

#endif
