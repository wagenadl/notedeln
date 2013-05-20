// FutileMovementInfo.C

#include "FutileMovementInfo.H"
#include "TextItem.H"
#include <QTextCursor>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextLine>

FutileMovementInfo::FutileMovementInfo() {
}

FutileMovementInfo::FutileMovementInfo(int key, Qt::KeyboardModifiers mod,
				       TextItem *src) {
  key_ = key;
  modifiers_ = mod;
  if (src) {
    QTextCursor c = src->textCursor();
    QTextBlock b = c.block();
    QTextLayout *lay = b.layout();
    QPointF xy0 = lay->position(); // in item
    int p = c.positionInBlock();
    QTextLine line = lay->lineForTextPosition(p);
    QPointF xy(line.cursorToX(p), line.y()+line.ascent()); // in layout
    scenePos_ = src->mapToScene(xy0 + xy);
  }
}


QPointF FutileMovementInfo::scenePos() const {
  return scenePos_;
}

int FutileMovementInfo::key() const {
  return key_;
}

Qt::KeyboardModifiers FutileMovementInfo::modifiers() const {
  return modifiers_;
}
