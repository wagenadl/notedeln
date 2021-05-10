// Scenes/FrontScene.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// FrontScene.C

#include "FrontScene.h"

#include "BookData.h"
#include "Notebook.h"
#include "Style.h"
#include "RoundedRect.h"
#include "Version.h"
#include "Translate.h"
#include "DefaultingQTI.h"

#include <math.h>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QCursor>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QPainter>
#include <QTextDocument>
#include <QGraphicsView>

FrontScene::FrontScene(Notebook *book, QObject *parent):
  QGraphicsScene(parent),
  book(book), style(book->style()) {
  makeBackground();
  makeItems();
  rebuild();
  if (!book->isReadOnly()) {
    makeWritable();
    if (book->bookData()->title()==Translate::_("New book")) {
      // QTextCursor t(title->document());
      // t.movePosition(QTextCursor::Start);
      // t.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
      // title->setTextCursor(t);
      // title->setFocus();
    }
  }
}

FrontScene::~FrontScene() {
}

void FrontScene::recolorItems() {
  title->setDefaultTextColor(style.color("front-title-color"));
  author->setDefaultTextColor(style.color("front-author-color"));
  address->setDefaultTextColor(style.color("front-address-color"));
}

void FrontScene::makeWritable() {
  QList<QGraphicsTextItem *> ll;
  ll << title << author << address;
  foreach (QGraphicsTextItem *ti, ll) {
    ti->setTextInteractionFlags(Qt::TextEditorInteraction);
    ti->setCursor(QCursor(Qt::IBeamCursor));
    ti->setFlag(QGraphicsTextItem::ItemIsFocusable);
    connect(ti->document(),
            SIGNAL(contentsChange(int, int, int)),
	    this, SLOT(textChange()));
  }
}

void FrontScene::textChange() {
  BookData *data = book->bookData();
  data->setTitle(title->toPlainText());
  data->setAuthor(author->toPlainText());
  data->setAddress(address->toPlainText());
  rebuildOItems();
  positionItems();
  recolorItems();
  foreach (QGraphicsView *view, views()) {
    QWidget *toplevel = view->window();
    if (toplevel)
      toplevel->setWindowTitle(title->toPlainText()
			       .replace(QRegExp("\\s\\s*"), " ") + " - eln");
  }
}

static void centerAt(QGraphicsItem *item, double x, double y) {
  QRectF bb = item->boundingRect();
  double x0 = (bb.left() + bb.right()) / 2;
  double y0 = (bb.top() + bb.bottom()) / 2;
  item->setPos(QPointF(x-x0, y-y0));
}


void FrontScene::rebuild() {
  BookData *data = book->bookData();
  title->setPlainText(data->title());
  author->setPlainText(data->author());
  address->setPlainText(data->address());
  redate();
  rebuildOItems();
  positionItems();
  recolorItems();
}

void FrontScene::redate() {
  BookData *data = book->bookData();
  QString dateFmt = style.string("front-date-format");
  QDate start = data->startDate();
  QDate end = book->endDate();
  if (start == end)
    dates->setHtml(start.toString(dateFmt));
  else
    dates->setHtml(start.toString(dateFmt)
		   + QString::fromUtf8("‒") +
		   end.toString(dateFmt));

  centerAt(dates, style.real("page-width") / 2, style.real("front-dates-y"));
}

static QString otext(QString lbl, QString txt) {
  if (txt.isEmpty())
    return "";
  return "(" + Translate::_(lbl) + QString::fromUtf8(": “")
    + txt + QString::fromUtf8("”)");
}

void FrontScene::rebuildOItems() {
  BookData *data = book->bookData();
  otitle->setPlainText(otext("otitle", data->otitle()));
  oauthor->setPlainText(otext("oauthor", data->oauthor()));
  oaddress->setPlainText(otext("oaddress", data->oaddress()));
}

void FrontScene::makeBackground() {
  bg = 0;
  
  setSceneRect(0,
	       0,
	       style.real("page-width"),
	       style.real("page-height"));
  
  setBackgroundBrush(QBrush(style.color("border-color")));
  
  addRect(0,
	  0,
	  style.real("page-width"),
	  style.real("page-height"),
	  QPen(Qt::NoPen),
	  QBrush(style.color("background-color")));

  QImage img(book->filePath("front.jpg"));
  bool customFront = !img.isNull();
  if (!customFront)
    img = QImage(":/front.jpg");
  
  if (!img.isNull()) {
    if (style.contains("front-recolor")) {
      QColor c(style.color("front-recolor"));
      unsigned char lookup[3][256];
      qreal cc[3]; c.getRgbF(cc, cc+1, cc+2);
      for (int k=0; k<3; k++) {
        cc[k] = pow(2.0, -4*(cc[k]-.5));
        for (int i=0; i<256; i++) {
          lookup[k][i] = (unsigned char)(255.99*pow((i/255.), cc[k]));
        }
      }

      img = img.convertToFormat(QImage::Format_ARGB32);
      int X = img.width();
      int Y = img.height();
      for (int y=0; y<Y; y++) {
        uchar *ptr = img.scanLine(y);
        for (int x=0; x<X; x++) {
          for (int k=0; k<3; k++) 
            ptr[k] = lookup[k][ptr[k]];
          ptr+=4;
        }
      }
    }
            
    bg = new QGraphicsPixmapItem();
    bg->setPixmap(QPixmap::fromImage(img));
    addItem(bg);
    QTransform t;
    t.scale(style.real("page-width")/img.width(),
	    style.real("page-height")/img.height());
    bg->setTransform(t);
  }

  if (customFront) {
    toprect = 0;
    bottomrect = 0;
  } else {
    toprect = new RoundedRect();
    addItem(toprect);
    bottomrect = new RoundedRect();
    addItem(bottomrect);
  }

  QString eln = Translate::_("eln");
  QGraphicsTextItem *dw
    = addText(eln
              + " v. " + Version::toString()
              + QString::fromUtf8(" — ")
	      + QString::fromUtf8("(C) Daniel Wagenaar 2013–")
	      + QString::number(Version::buildDate().year()),
	      style.font("splash-small-font"));
  centerAt(dw, style.real("page-width")/2, style.real("page-height") - 20);
}

void FrontScene::makeItems() {
  title = addDefaultingText(Translate::_("New book"),
			    style.font("front-title-font"),
			    style.color("front-title-color"));
  
  author = addDefaultingText(Translate::_("Me"),
			     style.font("front-author-font"),
			     style.color("front-author-color"));

  address = addDefaultingText(Translate::_("Here"),
			      style.font("front-address-font"),
			      style.color("front-address-color"));

  dates = addText("dates", style.font("front-dates-font"));
  dates->setDefaultTextColor(style.color("front-dates-color"));

  otitle = addText("", style.font("front-aka-font"));
  otitle->setDefaultTextColor(style.color("front-aka-color"));
  
  oauthor = addText("", style.font("front-aka-font"));
  oauthor->setDefaultTextColor(style.color("front-aka-color"));

  oaddress = addText("", style.font("front-aka-font"));
  oaddress->setDefaultTextColor(style.color("front-aka-color"));
}

void FrontScene::positionItems() {
  double xc = style.real("page-width") / 2;

  QList<DefaultingQTI *> lst; lst << title << author << address;
  foreach (DefaultingQTI *it, lst) {
    it->setTextWidth(-1);
    it->setTextWidth(it->boundingRect().width());
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignHCenter);
    QTextCursor c0 = it->textCursor();
    QTextCursor cursor = it->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    it->setTextCursor(cursor);
    it->setTextCursor(c0);
  }
  
  centerAt(title, xc, style.real("front-title-y"));
  centerAt(author, xc, style.real("front-author-y"));
  centerAt(address, xc, style.real("front-address-y"));
  centerAt(dates, xc, style.real("front-dates-y"));

  centerAt(otitle, xc,
	   title->mapRectToScene(title->boundingRect()).bottom() + 5);
  centerAt(oaddress, xc,
	   address->mapRectToScene(address->boundingRect()).bottom() + 5);
  centerAt(oauthor, xc,
	   author->mapRectToScene(author->boundingRect()).bottom() + 5);

  if (toprect) {
    QRectF tr = title->sceneBoundingRect();
    tr |= dates->sceneBoundingRect();
    tr.adjust(-36, -18, 36, 18); // to be improved
    toprect->setPos(tr.topLeft());
    toprect->resize(tr.size());
  }

  if (bottomrect) {
    QRectF br = author->sceneBoundingRect();
    br |= address->sceneBoundingRect();
    br.adjust(-36, -18, 36, 18); // to be improved
    bottomrect->setPos(br.topLeft());
    bottomrect->resize(br.size());
  } 
}

void FrontScene::print(QPrinter *, QPainter *p) {
  render(p);
}

DefaultingQTI *FrontScene::addDefaultingText(QString dflt, QFont f, QColor c) {
  DefaultingQTI *ti = new DefaultingQTI(dflt, 0);
  ti->setFont(f);
  ti->setDefaultTextColor(c);
  addItem(ti);
  return ti;
}
