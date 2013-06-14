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

#include "PageView.H"
#include "App.H"
#include "EntryScene.H"
#include "Notebook.H"
#include "EntryFile.H"
#include "DataFile.H"
#include "TOCScene.H"
#include "FrontScene.H"
#include "TitleData.H"
#include "DeletedStack.H"
#include "Assert.H"
#include "Toolbars.H"
//#include "SimpleNavbar.H"
#include "Navbar.H"
#include "Mode.H"
#include "BlockItem.H"
#include "TextItem.H"
#include "GfxNoteItem.H"

#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>

PageView::PageView(Notebook *nb, QWidget *parent):
  QGraphicsView(parent), book(nb) {
  toolbars = new Toolbars(mode(), 0); // toolbars is unparented except when viewing a page
  connect(toolbars->navbar(), SIGNAL(goTOC()), SLOT(gotoTOC()));
  connect(toolbars->navbar(), SIGNAL(goEnd()), SLOT(lastPage()));
  connect(toolbars->navbar(), SIGNAL(goRelative(int)), SLOT(goRelative(int)));
  deletedStack = new DeletedStack(this);
  frontScene = new FrontScene(nb, this);
  tocScene = new TOCScene(nb->toc(), this);
  tocScene->populate();
  // simpleNavbar = new SimpleNavbar(tocScene);
  // connect(simpleNavbar, SIGNAL(goRelative(int)), SLOT(goRelative(int)));
  connect(tocScene, SIGNAL(pageNumberClicked(int)),
          SLOT(gotoPage(int)));

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  pageScene = 0;
  currentSection = Front;

  wheelDeltaAccum = 0;
  wheelDeltaStepSize = nb ? nb->style().real("wheelstep") : 120;
}

PageView::~PageView() {
  if (!toolbars->scene())
    delete toolbars; // othw. the scene will take care of it
}

void PageView::resizeEvent(QResizeEvent *e) {
  QGraphicsView::resizeEvent(e);
  if (!scene())
    return;
  QRectF sr = scene()->sceneRect();
  sr.adjust(2, 2, -2, -2); // make sure no borders show by default
  fitInView(sr, Qt::KeepAspectRatio);
}

void PageView::mousePressEvent(QMouseEvent *e) {
  QGraphicsView::mousePressEvent(e);
}
  
void PageView::keyPressEvent(QKeyEvent *e) {
  bool take = true;
  switch (e->key()) {
  case Qt::Key_F1:
    mode()->setMode(Mode::Browse);
    break;
  case Qt::Key_F2:
    mode()->setMode(Mode::Type);
    break;
  case Qt::Key_F3:
    mode()->setMode(Mode::MoveResize);
    break;
  case Qt::Key_F4:
    mode()->setMode(Mode::Mark);
    break;
  case Qt::Key_F5:
    mode()->setMode(Mode::Freehand);
    break;
  case Qt::Key_F6:
    mode()->setMode(Mode::Annotate);
    break;
  case Qt::Key_F7:
    mode()->setMode(Mode::Highlight);
    break;
  case Qt::Key_F8:
    mode()->setMode(Mode::Strikeout);
    break;
  case Qt::Key_F9:
    mode()->setMode(Mode::Plain);
    break;
  case Qt::Key_PageUp:
    previousPage();
    break;
  case Qt::Key_PageDown:
    nextPage();
    break;
  case Qt::Key_Home:
    if (e->modifiers() & Qt::ControlModifier) 
      gotoTOC();
    else
      take = false;
    break;
  case Qt::Key_End:
    if (e->modifiers() & Qt::ControlModifier) 
      lastPage();
    else
      take = false;
    break;
  case Qt::Key_Delete:
    if (currentSection==Pages && mode()->mode()==Mode::MoveResize) {
      QPointF p = mapToScene(mapFromGlobal(QCursor::pos()));
      Item *item = 0;
      for (QGraphicsItem *gi = pageScene->itemAt(p); gi!=0;
	   gi = gi->parentItem()) {
	item = dynamic_cast<Item*>(gi);
	if (item)
	  break;
      }
      if (item && item->isWritable()) {
	BlockItem *block = item->ancestralBlock();
	if (block && block->allChildren().isEmpty())
	  block->pageScene()->notifyChildless(block);
	else
	  deletedStack->grabIfRestorable(item);
      }
    } else {
      take = false;
    }      
    break;
  case Qt::Key_Insert:
    if (currentSection==Pages && pageScene->focusItem()==0) 
      deletedStack->restoreTop();
    else
      take = false;
    break;
  case Qt::Key_P:
    if (e->modifiers() & Qt::ControlModifier) 
      printDialog();
    else
      take = false;
    break;
  case Qt::Key_C:
    if (currentSection==Pages
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
  switch (e->key()) {
  case Qt::Key_Alt:
    mode()->temporaryRelease(Mode::MoveResize);
    break;
  default:
    break;
  }
    
  QGraphicsView::keyReleaseEvent(e);
}

void PageView::nowOnPage(int n) {
  if (currentSection==Pages)
    currentPage = n;
}

void PageView::gotoPage(int n) {
  if (n<1)
    n=1;

  if (n>=book->toc()->newPageNumber()) {
    // make a new page?
    TOCEntry *te = book->toc()->find(n-1);
    if (te) {
      // let's look at page before end
      EntryFile *file = book->page(te->startPage());
      ASSERT(file);
      if (file->data()->isEmpty()) {
	// previous page is empty -> go there instead
	gotoPage(n-1);
	return;
      }
    }
    book->createPage(n);
  }

  if (currentSection==Pages && currentPage==n)
      return; // don't move (special case must be checked to avoid deleting tgt)
  
  TOCEntry *te = book->toc()->find(n);
  if (!te) {
    qDebug() << "PageEditor: gotoPage("<<n<<"): no such page";
    return;
  }

  if (currentSection==Pages
      && currentPage>=te->startPage()
      && currentPage<te->startPage()+te->sheetCount()) {
    // already in the right page, let's just go to the right sheet
    currentPage = n;
  } else {
    EntryFile *file = book->page(te->startPage());
    ASSERT(file);
    ASSERT(file->data());
    
    leavePage();
    if (pageScene)
      pageScene->deleteLater();
    pageScene = 0;
    
    currentSection = Pages;
    currentPage = n;
    
    pageScene = new EntryScene(file->data(), this);
    pageScene->populate();
    connect(pageScene, SIGNAL(nowOnPage(int)), SLOT(nowOnPage(int)));
    if (book->toc()->isLast(te) && file->data()->isRecent())
      pageScene->makeWritable(); // this should be even more sophisticated
    setScene(pageScene);
    pageScene->addItem(toolbars);
    toolbars->showTools();
  }
  
  pageScene->gotoSheet(currentPage - te->startPage());
  
  if (pageScene->data()->title()->isDefault())
    pageScene->focusTitle();
  else
    pageScene->focusEnd();

  if (pageScene->isWritable())
    mode()->setMode(Mode::Type);
  else
    mode()->setMode(Mode::Browse);    
}

void PageView::gotoFront() {
  leavePage();
  currentSection = Front;
  setScene(frontScene);
  frontScene->addItem(toolbars);
  toolbars->hideTools();
}

void PageView::gotoTOC(int n) {
  leavePage();
  
  currentSection = TOC;
  currentPage = n;
  setScene(tocScene);
  tocScene->gotoSheet(currentPage-1);
  tocScene->addItem(toolbars);
  toolbars->hideTools();
}

void PageView::leavePage() {
  QGraphicsScene *s = scene();
  if (s) {
    QGraphicsItem *fi = s->focusItem();
    if (fi)
      fi->clearFocus(); // this should cause abandon to happen
  }

  if (toolbars->scene())
    toolbars->scene()->removeItem(toolbars);

  if (currentSection==Pages
      && currentPage>1
      && currentPage==book->toc()->newPageNumber()-1) {
    // Leaving the last page in the notebook, not being the only page.
    // If the page is empty, we'll delete it.   
    TOCEntry *te = book->toc()->find(currentPage);
    ASSERT(te);
    EntryFile *file = book->page(te->startPage());
    ASSERT(file);
    ASSERT(file->data());
    if (file->data()->isEmpty()) {
      // Leaving an empty page
      if (pageScene)
	pageScene->deleteLater();
      pageScene = 0;
      book->deletePage(currentPage);
    }
  }
}  

void PageView::previousPage() {
  switch (currentSection) {
  case Front:
    break;
  case TOC:
    if (currentPage<=1)
      gotoFront();
    else
      gotoTOC(currentPage-1);
    break;
  case Pages:
    if (currentPage<=1)
      gotoTOC(tocScene->sheetCount());
    else
      gotoPage(currentPage-1);
    break;
  }
}

void PageView::goRelative(int n) {
  switch (currentSection) {
  case Front:
    break;
  case TOC:
    n += currentPage;
    break;
  case Pages:
    n += tocScene->sheetCount() + currentPage;
    break;
  }
  
  // now n is an absolute page number; title=0; toc=1..N; pages=N+1..
  if (n<=0) {
    gotoFront();
    return;
  }

  // n=1 is the first toc page
  if (n<=tocScene->sheetCount()) {
    gotoTOC(n);
    return;
  }

  n -= tocScene->sheetCount(); // now n=1 is the first page
  if (n<book->toc()->newPageNumber())
    gotoPage(n);
  else
    gotoPage(book->toc()->newPageNumber()); // create new unless prev. empty
}

void PageView::nextPage() {
  switch (currentSection) {
  case Front:
    gotoTOC();
    break;
  case TOC:
    if (currentPage>=tocScene->sheetCount())
      gotoPage(1);
    else
      gotoTOC(currentPage+1);
    break;
  case Pages:
    if (currentPage>=book->toc()->newPageNumber()) {
      // go to index?
    } else if (currentPage==book->toc()->newPageNumber()-1) {
      gotoPage(currentPage+1);
      // makes a new page unless current is empty
    } else {
      gotoPage(currentPage+1);
    }
    break;
  }
}

void PageView::lastPage() {
  gotoPage(book->toc()->newPageNumber()-1);
}

Mode *PageView::mode() const {
  ASSERT(book);
  return book->mode();
}

void PageView::wheelEvent(QWheelEvent *e) {
  wheelDeltaAccum += e->delta();
  qDebug() << "pageview:mousewheel delta=" << e->delta()
           << "accum=" << wheelDeltaAccum;
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
  QString newTtl = pageScene->data()->title()->current()->text();
  if (!newTtl.endsWith(" (cont’d)"))
    newTtl += " (cont’d)";
  int oldPage = pageScene->startPage() + pageScene->currentSheet();
  int newPage = book->toc()->newPageNumber();
  Style const &style = book->style();

  qDebug() << "oldPage" << oldPage << "newPage"<<newPage << pageScene;
  
  // Create forward note
  QPointF fwdNotePos(style.real("page-width")/2,
                     style.real("page-height")
                     - style.real("margin-bottom")
                     + style.real("pgno-sep"));
  qDebug() << fwdNotePos;
  GfxNoteItem *fwdNote = pageScene->newNote(fwdNotePos);
  qDebug() << fwdNote;
  TextItem *fwdNoteTI = fwdNote->textItem();
  QTextCursor cursor = fwdNoteTI->textCursor();
  QString fwdNoteText = QString("(see p. %1)").arg(newPage);
  cursor.insertText(fwdNoteText);
  cursor.setPosition(fwdNoteText.size()-1);
  fwdNoteTI->setTextCursor(cursor);
  fwdNoteTI->tryExplicitLink(); // dead at first, but not for long
  QPointF pp = fwdNote->mapToScene(fwdNote->netBounds().topLeft());
  fwdNote->translate(fwdNotePos - pp);

  // Goto new page
  gotoPage(newPage);
  ASSERT(pageScene);
  // (So now pageScene refers to the new page.)
  
  // Create reverse note
  QPointF revNotePos(style.real("margin-left"),
                     style.real("margin-top"));
  GfxNoteItem *revNote = pageScene->newNote(revNotePos);
  TextItem *revNoteTI = revNote->textItem();
  cursor = revNoteTI->textCursor();
  QString revNoteText = QString("p. %1 >").arg(oldPage);
  cursor.insertText(revNoteText);
  cursor.setPosition(revNoteText.size()-2);
  revNoteTI->setTextCursor(cursor);
  revNoteTI->tryExplicitLink();
  pp = revNote->mapToScene(revNote->netBounds().topRight());
  revNote->translate(revNotePos - pp);

  mode()->setMode(Mode::Type);
  pageScene->focusTitle();
}
