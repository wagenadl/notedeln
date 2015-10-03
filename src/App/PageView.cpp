// App/PageView.cpp - This file is part of eln

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

// PageView.C

#include "PageView.h"
#include "PageEditor.h"
#include "App.h"
#include "EntryScene.h"
#include "EntryData.h"
#include "Notebook.h"
#include "SceneBank.h"
#include "TOCScene.h"
#include "FrontScene.h"
#include "TitleData.h"
#include "DeletedStack.h"
#include "Cursors.h"
#include "Assert.h"
#include "Mode.h"
#include "BlockItem.h"
#include "TextBlockItem.h"
#include "TextItem.h"
#include "GfxNoteItem.h"
#include "SearchDialog.h"
#include "HtmlOutput.h"
#include "SheetScene.h"
#include "TitleItem.h"

#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QFileDialog>

PageView::PageView(SceneBank *bank, PageEditor *parent):
  QGraphicsView(parent), bank(bank) {
  book = bank->book(); // for convenience only
  ASSERT(book);
  ASSERT(parent);
  mode_ = new Mode(book->isReadOnly(), this);
  searchDialog = new SearchDialog(this);
  deletedStack = new DeletedStack(this);

  setFrameStyle(Raised | StyledPanel);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setDragMode(NoDrag);
  
  currentSection = Front;
  currentPage = 0; 
  currentSheet = 0;

  wheelDeltaAccum = 0;
  wheelDeltaStepSize = book->style().real("wheelstep");

  connect(mode(), SIGNAL(modeChanged(Mode::M)), SLOT(modeChange()));

  setAcceptDrops(true);

#if defined(Q_OS_ANDROID)
  setAttribute(Qt::WA_InputMethodEnabled);
  setAttribute(Qt::WA_KeyCompression, false);
  setInputMethodHints(Qt::ImhMultiLine);
#endif
}

PageView::~PageView() {
  leavePage();
}

void PageView::resizeEvent(QResizeEvent *e) {
  QGraphicsView::resizeEvent(e);
  if (!scene())
    return;
  QRectF r = scene()->sceneRect();
  fitInView(r.adjusted(1, 1, -2, -2),
	    Qt::KeepAspectRatio);
  emit scaled(matrix().m11());
}

void PageView::handleSheetRequest(int n) {
  qDebug() << "PageView" << this << "handleSheetRequest" << n;
  BaseScene *sc = 0;
  switch (currentSection) {
  case Front:
    break;
  case TOC:
    sc = bank->tocScene();
    break;
  case Entries:
    sc = entryScene.obj();
    qDebug() << "  entryscene startpage=" << entryScene->startPage();
    break;
  }
  if (sc && currentSheet < sc->sheetCount()) {
    qDebug() << "  got scene" << sc << "currentSheet=" << currentSheet << "count==" << sc->sheetCount();
    PageView *ev = sc->eventView();
    qDebug() << "eventview=" << ev;
    if (/*ev &&*/ ev!=this) {
      qDebug() << "  eventview mismatch -> returning";
      return;
    }
  }
  qDebug() << "  pageview" << this << "going to sheet" << n;
  gotoSheet(n);
}

bool PageView::gotoSheet(int n) {
  if (n<0)
    return false;
  switch (currentSection) {
  case Front:
    if (n>0)
      return false;
    currentPage = 0;
    setScene(bank->frontScene());
    emit onFrontMatter(currentPage);
    break;
  case TOC:
    if (n>=bank->tocScene()->sheetCount())
      return false;
    currentPage = 1+n;
    currentSheet = n;
    bank->tocScene()->sheet(n)->setEventView(this);
    setScene(bank->tocScene()->sheet(n));
    emit onFrontMatter(currentPage);
    return true;
  case Entries:
    if (n>=entryScene->sheetCount())
      return false;
    currentSheet = n;
    currentPage = entryScene->startPage() + n;
    entryScene->sheet(n)->setEventView(this);
    setScene(entryScene->sheet(n));
    emit onEntryPage(currentPage-n, n);
    return true;
  }
  return false;
}

void PageView::mousePressEvent(QMouseEvent *e) {
  markEventView();
  QGraphicsView::mousePressEvent(e);
}

void PageView::dragEnterEvent(QDragEnterEvent *e) {
  markEventView();
  QGraphicsView::dragEnterEvent(e);
}

void PageView::enterEvent(QEvent *e) {
  markEventView();
  modeChange();
  QGraphicsView::enterEvent(e);
}

void PageView::inputMethodEvent(QInputMethodEvent *e) {
  markEventView();
  QGraphicsView::inputMethodEvent(e);
}

void PageView::markEventView() {
  switch (currentSection) {
  case Front:
    break;
  case TOC:
    bank->tocScene()->setEventView(this);
    bank->tocScene()->sheet(currentSheet)->setEventView(this);
    break;
  case Entries:
    entryScene->setEventView(this);
    entryScene->sheet(currentSheet)->setEventView(this);
    break;
  }
}  
  
void PageView::keyPressEvent(QKeyEvent *e) {
  markEventView();
  bool take = true;
  switch (e->key()) {
  case Qt::Key_F1:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    break;
  case Qt::Key_F2:
    if (currentSection==Entries) {
      mode()->setMode(Mode::Type);
      mode()->setMathMode(e->modifiers() & Qt::ShiftModifier);
    }
    break;
  case Qt::Key_F3:
    if (currentSection==Entries)
      mode()->setMode(Mode::MoveResize);
    break;
  case Qt::Key_F4:
    if (currentSection==Entries)
      mode()->setMode(Mode::Mark);
    break;
  case Qt::Key_F5:
    if (currentSection==Entries)
      mode()->setMode(Mode::Freehand);
    break;
  case Qt::Key_F6:
    if (currentSection==Entries)
      mode()->setMode(Mode::Annotate);
    break;
  case Qt::Key_F7:
    if (currentSection==Entries)
      mode()->setMode(Mode::Highlight);
    break;
  case Qt::Key_F8:
    if (currentSection==Entries)
      mode()->setMode(Mode::Strikeout);
    break;
  case Qt::Key_F9:
    if (currentSection==Entries)
      mode()->setMode(Mode::Plain);
    break;
  case Qt::Key_QuoteLeft: case Qt::Key_AsciiTilde: case Qt::Key_4:
    if (e->modifiers() & Qt::ControlModifier)
      mode()->setMathMode(!mode()->mathMode());
    else
      take = false;
    break;
  case Qt::Key_Space: case Qt::Key_Down: case Qt::Key_Right:
    if (mode()->mode()==Mode::Browse && !scene()->focusItem())
      nextPage();
    else
      take = false;
    break;
  case Qt::Key_Backspace: case Qt::Key_Up: case Qt::Key_Left:
    if (mode()->mode()==Mode::Browse  && !scene()->focusItem()) 
      previousPage();
    else
      take = false;
    break;
  case Qt::Key_PageUp:
    goRelative((e->modifiers() & Qt::ControlModifier) ? -10 : -1,
	       e->modifiers());
    break;
  case Qt::Key_PageDown:
    goRelative((e->modifiers() & Qt::ControlModifier) ? 10 : 1,
	       e->modifiers());
    break;
  case Qt::Key_Home:
    if ((e->modifiers() & Qt::ControlModifier) || mode()->mode()==Mode::Browse)
      goTOC(e->modifiers());
    else
      take = false;
    break;
  case Qt::Key_End:
    if ((e->modifiers() & Qt::ControlModifier) || mode()->mode()==Mode::Browse)
      lastPage(e->modifiers());
    else
      take = false;
    break;
  case Qt::Key_Delete:
    if (currentSection==Entries && mode()->mode()==Mode::MoveResize) {
      QPointF p = mapToScene(mapFromGlobal(QCursor::pos()));
      Item *item = 0;
      for (QGraphicsItem *gi = entryScene->itemAt(p, currentSheet); gi!=0;
	   gi = gi->parentItem()) {
	item = dynamic_cast<Item*>(gi);
	if (item)
	  break;
      }
      if (item && item->isWritable()) {
	BlockItem *block = item->ancestralBlock();
	if (block && block->allChildren().isEmpty())
	  entryScene->notifyChildless(block);
	else
	  deletedStack->grabIfRestorable(item);
      }
    } else {
      take = false;
    }      
    break;
  case Qt::Key_Insert:
    if (currentSection==Entries && scene()->focusItem()==0) 
      deletedStack->restoreTop();
    else
      take = false;
    break;
  case Qt::Key_F:
    if (e->modifiers() & Qt::ControlModifier)
      openFindDialog();
    else
      take = false;
    break;
  case Qt::Key_S:
    if ((e->modifiers() & Qt::ControlModifier)
	&& (e->modifiers() & Qt::ShiftModifier))
      htmlDialog();
    else
      take = false;
    break;
  case Qt::Key_T:
    if (currentSection==Entries
        && (e->modifiers() & Qt::ControlModifier)
	&& (e->modifiers() & Qt::ShiftModifier)) {
      int st = entryScene->data()->stampTime();
      if (entryScene->style().real("auto-timestamp-min-dt") > 1) {
	if (st>0)
	  st = -1;
	else if (st<0)
	  st = 0;
	else
	  st = 1;
      } else {
	if (st>0)
	  st = -1;
	else
	  st = 1;
      }
      entryScene->data()->setStampTime(st);
      entryScene->redateBlocks();
    } else {
      take = false;
    }
    break;
  case Qt::Key_P:
    if (e->modifiers() & Qt::ControlModifier) 
      openPrintDialog();
    else
      take = false;
    break;
  case Qt::Key_C:
    if (currentSection==Entries
        && (e->modifiers() & Qt::ControlModifier) 
        && (e->modifiers() & Qt::ShiftModifier))
      createContinuationEntry();
    else
      take = false;
    break;
  case Qt::Key_Alt:
    mode()->temporaryOverride(Mode::MoveResize);
    take = false;
    break;
  default:
    take = false;
    break;
  }
  
  if (take)
    e->accept();
  else    
    QGraphicsView::keyPressEvent(e);
}

void PageView::keyReleaseEvent(QKeyEvent *e) {
  markEventView();
  switch (e->key()) {
  case Qt::Key_Alt:
    mode()->temporaryRelease(Mode::MoveResize);
    break;
  default:
    break;
  }
    
  QGraphicsView::keyReleaseEvent(e);
}

void PageView::pageNumberClick(int n, Qt::KeyboardModifiers m) {
  if (m & Qt::ShiftModifier)
    newView()->gotoEntryPage(n);
  else
    gotoEntryPage(n);
}

void PageView::goTOC(Qt::KeyboardModifiers m) {
  if (m & Qt::ShiftModifier)
    newView()->gotoTOC();
  else
    gotoTOC();
}

PageView *PageView::newView() {
  PageEditor *myEditor = dynamic_cast<PageEditor *>(parentWidget());
  ASSERT(myEditor);
  PageEditor *newEditor = myEditor->newEditor();
  newEditor->resize(myEditor->size());
  return myEditor->pageView();
}  

void PageView::gotoEntryPage(QString s) {
  if (s.isEmpty()) {
    gotoEntryPage(0);
  } else {
    if (s[s.size()-1]>='a') {
      int p0 = s.left(s.size()-1).toInt();
      int dp = 1 + s.at(s.size()-1).unicode() - 'a';
      gotoEntryPage(p0);
      gotoSheet(currentSheet + dp);
    } else {
      gotoEntryPage(s.toInt());
    }
  }
}

void PageView::gotoEntryPage(int n, int dir) {
  if (n<1)
    n=1;
  int N = book->toc()->newPageNumber();
  if (n>N)
    n=N;

  if (n==N) {
    // make a new page?
    TOCEntry *te = book->toc()->findBackward(n);
    if (te) {
      // let's look at page before end
      CachedEntry ef(book->entry(te->startPage()));
      ASSERT(ef);
      if (ef->isEmpty()) {
	// previous page is empty -> go there instead
	gotoEntryPage(te->startPage() + te->sheetCount() - 1);
	return;
      }
    }
    book->createEntry(n);
  }

  TOCEntry *te = book->toc()->find(n);
  if (!te) {
    if (dir<0)
      te = book->toc()->findBackward(n);
    else if (dir>0)
      te = book->toc()->findForward(n);
    if (!te) {
      return;
    }
  }

  if (currentSection==Entries
      && book->toc()->find(currentPage-currentSheet)==te) {
    // already in the right page, let's just go to the right sheet
  } else {
    leavePage();
    entryScene = bank->entryScene(te->startPage());

    connect(entryScene.obj(), SIGNAL(sheetRequest(int)),
	    SLOT(handleSheetRequest(int)));
    if (entryScene->data()->isWritable())
      entryScene->makeWritable(); // this should be even more sophisticated
    currentSection = Entries;
  }
  currentPage = n;

  gotoSheet(currentPage - te->startPage());
  
  if (entryScene->data()->title()->isDefault())
    entryScene->focusTitle(currentSheet);

  if (entryScene->isWritable())
    mode()->setMode(Mode::Type);
  else
    mode()->setMode(Mode::Browse);
}

void PageView::gotoFront() {
  leavePage();
  currentSection = Front;
  gotoSheet(0);
}

void PageView::gotoTOC(int n) {
  leavePage();
  currentSection = TOC;
  gotoSheet(n-1);
}

void PageView::leavePage() {
  QGraphicsScene *s = scene();
  if (s) {
    QGraphicsItem *fi = s->focusItem();
    if (fi)
      fi->clearFocus(); // this should cause abandon to happen
  }

  if (currentSection==Entries
      && currentPage==book->toc()->newPageNumber()-1) {
    // Leaving the last page in the notebook.
    // If the page is empty, we'll delete it.
    if (entryScene->data()->isEmpty()) {
      // Leaving an empty page
      QList<QGraphicsView *> allv = entryScene->allViews();
      if (allv.size()==1 && allv.first() == this) {
	entryScene.clear();
	book->deleteEntry(currentPage);
	return;
      }
    } 
    book->flush();
  }
}  

void PageView::previousPage() {
  if (scene())
    scene()->clearFocus();
  switch (currentSection) {
  case Front:
    break;
  case TOC:
    if (!gotoSheet(currentSheet-1))
      gotoFront();
    break;
  case Entries:
    if (!gotoSheet(currentSheet-1)) {
      if (currentPage<=1) {
        if (book->toc()->newPageNumber()<=2
            && entryScene->data()->isEmpty())
          gotoFront();
        else
          gotoTOC(bank->tocScene()->sheetCount());
      } else {
	gotoEntryPage(currentPage-1, -1);
	gotoSheet(entryScene->sheetCount()-1);
	// the "gotoSheet" call ensures we go to continuation pages
      }
    }
    break;
  }
  focusEntry();
}


void PageView::goRelative(int n, Qt::KeyboardModifiers m) {
  if (m & Qt::ShiftModifier) {
    newView()->goRelative(n);
    return;
  }
  int dir = n;
  if (n==1) {
    /* This magic is to deal with continuation pages ("93a"). */
    nextPage();
    return;
  } else if (n==-1) {
    previousPage();
    return;
  }

  int N = bank->tocScene()->sheetCount();
  
  switch (currentSection) {
  case Front:
    break;
  case TOC:
    n += currentPage;
    break;
  case Entries:
    n += N + currentPage;
    break;
  }
  
  // now n is an "absolute" page number; title=0; toc=1..N; pages=N+1..
  if (n<=0) {
    gotoFront();
    return;
  }

  // n=1 is the first toc page
  if (n<=bank->tocScene()->sheetCount()) {
    gotoTOC(n);
    return;
  }

  n -= N; // now n=1 is the first page
  gotoEntryPage(n, dir); // may create new unless prev empty
  focusEntry();
}

void PageView::nextPage() {
  if (scene())
    scene()->clearFocus();
  switch (currentSection) {
  case Front:
    if (book->toc()->newPageNumber()==1)
      gotoEntryPage(1, 1);
    else
      gotoTOC();
    break;
  case TOC:
    if (!gotoSheet(currentSheet+1))
      gotoEntryPage(1, 1);
    break;
  case Entries:
    if (!gotoSheet(currentSheet+1)) {
      TOCEntry *te = book->toc()->tocEntry(entryScene->data()->startPage());
      te = book->toc()->entryAfter(te);
      if (te)
	gotoEntryPage(te->startPage(), 1);
      else
	gotoEntryPage(currentPage+1, 1); // this may make a new page at the end
    }
    break;
  }
  focusEntry();
}

void PageView::focusEntry() {
  if (currentSection==Entries) {
    if (entryScene->data()->title()->isDefault())
      entryScene->focusTitle(currentSheet);
    else
      entryScene->focusEnd(currentSheet);
  }
}

void PageView::lastPage(Qt::KeyboardModifiers m) {
  if (m & Qt::ShiftModifier) {
    newView()->lastPage();
  } else {
    gotoEntryPage(book->toc()->newPageNumber()-1);
    gotoSheet(entryScene->sheetCount()-1);
    focusEntry();
  }
}

Mode *PageView::mode() const {
  return mode_;
}

Notebook *PageView::notebook() const {
  return book;
}

void PageView::wheelEvent(QWheelEvent *e) {
  markEventView();
  wheelDeltaAccum += e->delta();
  int step = (e->modifiers() & Qt::ShiftModifier) ? 10 : 1;
  while (wheelDeltaAccum>=wheelDeltaStepSize) {
    wheelDeltaAccum -= wheelDeltaStepSize;
    goRelative(-step);
  }
  while (wheelDeltaAccum<=-wheelDeltaStepSize) {
    wheelDeltaAccum += wheelDeltaStepSize;
    goRelative(step);
  }
}

void PageView::createContinuationEntry() {
  QString newTtl = entryScene->data()->title()->text()->text();
  if (!newTtl.endsWith(QString::fromUtf8(" (cont’d)")))
    newTtl += QString::fromUtf8(" (cont’d)");
  int oldPage = entryScene->startPage() + currentSheet;
  int newPage = book->toc()->newPageNumber();
  Style const &style = book->style();

  // Create forward note
  QPointF fwdNotePos(style.real("page-width")/2,
                     style.real("page-height")
                     - style.real("margin-bottom")
                     + style.real("pgno-sep"));
  GfxNoteItem *fwdNote = entryScene->newNote(currentSheet, fwdNotePos);
  TextItem *fwdNoteTI = fwdNote->textItem();
  TextCursor cursor = fwdNoteTI->textCursor();
  QString fwdNoteText = QString("(see p. %1)").arg(newPage);
  cursor.insertText(fwdNoteText);
  cursor.setPosition(fwdNoteText.size()-1);
  fwdNoteTI->setTextCursor(cursor);
  fwdNoteTI->tryExplicitLink(); // dead at first, but not for long
  QPointF pp = fwdNote->mapToScene(fwdNote->netBounds().topLeft());
  fwdNote->translate(fwdNotePos - pp);

  // Goto new page
  gotoEntryPage(newPage);
  ASSERT(entryScene);
  // (So now entryScene refers to the new page.)
  entryScene->data()->title()->text()->setText(newTtl);
  TextItem *ti = entryScene->sheet(0)->fancyTitleItem();
  if (ti)
    ti->document()->relayout();
  
  // Create reverse note
  QPointF revNotePos(style.real("margin-left"),
                     style.real("margin-top"));
  GfxNoteItem *revNote = entryScene->newNote(currentSheet, revNotePos);
  TextItem *revNoteTI = revNote->textItem();
  revNoteTI->setTextWidth(0);
  cursor = revNoteTI->textCursor();
  QString revNoteText = QString("p. %1 >").arg(oldPage);
  cursor.insertText(revNoteText);
  cursor.setPosition(revNoteText.size()-2);
  revNoteTI->setTextCursor(cursor);
  revNoteTI->tryExplicitLink();
  pp = revNote->mapToScene(revNote->netBounds().topRight());
  qDebug() << "revnote: netbounds=" << revNote->netBounds() << "pos=" << revNote->pos();
  qDebug() << "  topright=" << pp << " desired="<<revNotePos; 
  revNote->translate(revNotePos - pp);

  gotoEntryPage(oldPage);
  gotoEntryPage(newPage); // pick up new title

  mode()->setMode(Mode::Type);
  entryScene->focusTitle(0);
}

void PageView::openFindDialog() {
  searchDialog->newSearch();
}

void PageView::htmlDialog() {
  if (currentSection==Entries) {
    QString fn = QFileDialog::getSaveFileName(this, "Save entry as html",
                                              QString("%1.html")
                                              .arg(entryScene->startPage()),
                                              "Web pages (*.html)");
    if (!fn.isEmpty()) {
      if (!fn.endsWith(".html"))
        fn += ".html";
      HtmlOutput html(fn, entryScene->title());
      html.add(entryScene.obj());
    }
  }
}

PageView::Section PageView::section() const {
  return currentSection;
}

QString PageView::pageName() const {
  switch (currentSection) {
  case Front:
    return "0";
  case TOC:
    return bank->tocScene()->pgNoToString(currentPage);
  case Entries:
    return entryScene->pgNoToString(currentPage);
  }
  return "";
}

int PageView::pageNumber() const {
  return currentPage;
}

void PageView::modeChange() {
  if (currentSection!=Entries)
    return;
  SheetScene *ss = entryScene->sheet(currentSheet);
  if (ss)
    ss->setCursors();
  QPointF p = mapToScene(mapFromGlobal(QCursor::pos()));
  Item *item = dynamic_cast<Item*>(entryScene->itemAt(p, currentSheet));
  if (item) {
    item->setCursor(Cursors::refined(item->cursorShape()));
    item->modeChangeUnderCursor();
  }
}

void PageView::drop(QDropEvent e) {
  dropEvent(&e);
}

void PageView::ensureSearchVisible(QString uuid, QString phrase) {
  scene()->update();  
  if (currentSection!=Entries)
    return;
  BlockItem const *blki = entryScene->findBlockByUUID(uuid);
  if (!blki) {
    qDebug() << "EnsureSearchVisible: block not found";
    return;
  }
  TextBlockItem const *tbi = dynamic_cast<TextBlockItem const *>(blki);
  if (!tbi) {
    qDebug() << "EnsureSearchVisible: not a text block";
    return;
  }
  int ifr = tbi->findFragmentForPhrase(phrase);
  if (ifr<0) {
    qDebug() << "EnsureSearchVisible: phrase not found in block!?";
    return;
  }
  while (ifr>0) {
    nextPage();
    --ifr;
  }
}

void PageView::focusInEvent(QFocusEvent *e) {
  markEventView();
  QGraphicsView::focusInEvent(e);
  update(); // ensure text cursor looks ok
}

void PageView::focusOutEvent(QFocusEvent *e) {
  QGraphicsView::focusOutEvent(e);
  update(); // ensure text cursor looks ok
}

void PageView::drawBackground(QPainter *p, QRectF const &r) {
  cursorDrawer.pos = QPointF(); // invalidate
  QGraphicsView::drawBackground(p, r);
}

void PageView::drawForeground(QPainter *p, QRectF const &r) {
  QGraphicsView::drawForeground(p, r);
  if (hasFocus() && !cursorDrawer.pos.isNull()) {
    p->setPen(cursorDrawer.color);
    p->setFont(cursorDrawer.font);
    p->drawText(cursorDrawer.pos, "|");
  }
}

void PageView::markCursor(QPointF p, QFont f, QColor c) {
  cursorDrawer.font = f;
  cursorDrawer.color = c;
  cursorDrawer.pos = p;
}
