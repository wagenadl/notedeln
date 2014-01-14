// FindOverlay.cpp

#include "FindOverlay.H"
#include <QGraphicsScene>
#include "TextItem.H"
#include "MarkupData.H"
#include "FindRegion.H"
#include <QDebug>

FindOverlay::FindOverlay(QGraphicsScene *scene, QString phrase) {
  qDebug() << "FindOverlay" << this;
  setZValue(-1);
  foreach (QGraphicsItem *i, scene->items()) {
    TextItem *ti = dynamic_cast<TextItem *>(i);
    if (ti && ti->isVisible()) {
      int idx = -1;
      QString txt = ti->data()->text();
      while (true) {
	idx = txt.indexOf(phrase, idx+1);
	if (idx<0)
	  break;
	MarkupData *md = new MarkupData(idx, idx+phrase.size(),
					MarkupData::Emphasize, 0);
	QObject *obj = md; obj->setParent(this); // so it will be destroyed
	new FindRegion(md, ti, this);
      }
    }
  }

  //  startTimer(10000); // self-destruct after 10 seconds

}

FindOverlay::~FindOverlay() {
  qDebug() << "~FindOverlay" << this;
  // children automatically destructed as well
}

void FindOverlay::timerEvent(QTimerEvent *) { // self-destruct
  deleteLater();
}


