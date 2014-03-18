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
#include "SearchDialog.H"
#include "HtmlOutput.H"

#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QFileDialog>

PageView::PageView(Notebook *nb, QWidget *parent):
  QGraphicsView(parent), book(nb) {
  toolbars = new Toolbars(mode(), 0); // toolbars is unparented except when viewing a page
  connect(toolbars->navbar(), SIGNAL(goTOC()), SLOT(gotoTOC()));
  connect(toolbars->navbar(), SIGNAL(goFind()), SLOT(openFindDialog()));
  connect(toolbars->navbar(), SIGNAL(goEnd()), SLOT(lastPage()));
  connect(toolbars->navbar(), SIGNAL(goRelative(int)), SLOT(goRelative(int)));
  searchDialog = new SearchDialog(this);
  deletedStack = new DeletedStack(this);
  frontScene = new FrontScene(nb, this);
  tocScene = new TOCScene(nb->toc(), this);
  tocScene->populate();
  // simpleNavbar = new SimpleNavbar(tocScene);
  // connect(simpleNavbar, SIGNAL(goRelative(int)), SLOT(goRelative(int)));
  connect(tocScene, SIGNAL(pageNumberClicked(int)),
          SLOT(gotoEntryPage(int)));

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  entryScene = 0;
  currentSection = Front;
  currentPage = 0; // I think that is correct 12/11/'13

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
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    break;
  case Qt::Key_F2:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    mode()->setMode(Mode::Type);
    break;
  case Qt::Key_F3:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    mode()->setMode(Mode::MoveResize);
    break;
  case Qt::Key_F4:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    mode()->setMode(Mode::Mark);
    break;
  case Qt::Key_F5:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    mode()->setMode(Mode::Freehand);
    break;
  case Qt::Key_F6:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    mode()->setMode(Mode::Annotate);
    break;
  case Qt::Key_F7:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    mode()->setMode(Mode::Highlight);
    break;
  case Qt::Key_F8:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    mode()->setMode(Mode::Strikeout);
    break;
  case Qt::Key_F9:
    if (currentSection==Entries)
      mode()->setMode(Mode::Browse);
    mode()->setMode(Mode::Plain);
    break;
  case Qt::Key_QuoteLeft: case Qt::Key_AsciiTilde:
    qDebug() << "QL" << e->key();
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
    previousPage();
    break;
  case Qt::Key_PageDown:
    qDebug() << "PageView::pagedown";
    nextPage();
    break;
  case Qt::Key_Home:
    if ((e->modifiers() & Qt::ControlModifier) || mode()->mode()==Mode::Browse)
      gotoTOC();
    else
      take = false;
    break;
  case Qt::Key_End:
    if ((e->modifiers() & Qt::ControlModifier) || mode()->mode()==Mode::Browse)
      lastPage();
    else
      take = false;
    break;
  case Qt::Key_Delete:
    if (currentSection==Entries && mode()->mode()==Mode::MoveResize) {
      QPointF p = mapToScene(mapFromGlobal(QCursor::pos()));
      Item *item = 0;
      for (QGraphicsItem *gi = entryScene->itemAt(p); gi!=0;
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
    if (currentSection==Entries && entryScene->focusItem()==0) 
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
  case Qt::Key_P:
    if (e->modifiers() & Qt::ControlModifier) 
      printDialog();
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
  if (currentSection==Entries) {
    currentPage = n;
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
      EntryFile *file = book->entry(te->startPage());
      ASSERT(file);
      if (file->data()->isEmpty()) {
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
      qDebug() << "PageEditor: gotoEntryPage("<<n<<"): no such page";
      return;
    }
  }

  if (currentSection==Entries && book->toc()->find(currentPage)==te) {
    // already in the right page, let's just go to the right sheet
    currentPage = n;
  } else {
    EntryFile *file = book->entry(te->startPage());
    ASSERT(file);
    ASSERT(file->data());
    
    leavePage();
    if (entryScene)
      entryScene->deleteLater();
    entryScene = 0;
    
    currentSection = Entries;
    currentPage = n;
    
    entryScene = new EntryScene(file->data(), this);
    entryScene->populate();
    connect(entryScene, SIGNAL(nowOnPage(int)), SLOT(nowOnPage(int)));
    if (file->data()->isRecent() || file->data()->isUnlocked())
      entryScene->makeWritable(); // this should be even more sophisticated
    setScene(entryScene);
    entryScene->addItem(toolbars);
    toolbars->showTools();
    TOCEntry *nextte = book->toc()->entryAfter(te);
    if (nextte)
      entryScene->clipPgNoAt(nextte->startPage());
  }

  entryScene->gotoSheet(currentPage - te->startPage());
  
  //if (entryScene->data()->title()->isDefault())
  //  entryScene->focusTitle();
  //else
  //  entryScene->focusEnd();

  if (entryScene->isWritable())
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

  if (currentSection==Entries
      && currentPage>1
      && currentPage==book->toc()->newPageNumber()-1) {
    // Leaving the last page in the notebook, not being the only page.
    // If the page is empty, we'll delete it.   
    TOCEntry *te = book->toc()->find(currentPage);
    ASSERT(te);
    EntryFile *file = book->entry(te->startPage());
    ASSERT(file);
    ASSERT(file->data());
    if (file->data()->isEmpty()) {
      // Leaving an empty page
      if (entryScene)
	entryScene->deleteLater();
      entryScene = 0;
      book->deleteEntry(currentPage);
    }
  }
}  

void PageView::previousPage() {
  if (scene())
    scene()->clearFocus();
  switch (currentSection) {
  case Front:
    break;
  case TOC:
    if (!tocScene->previousSheet())
      gotoFront();
    break;
  case Entries:
    if (!entryScene->previousSheet()) {
      if (currentPage<=1) {
        gotoTOC(tocScene->sheetCount());
      } else {
        gotoEntryPage(currentPage-1, -1);
        entryScene->gotoSheet(entryScene->sheetCount()-1);
        // the "gotoSheet" call ensures we go to continuation pages
      }
    }
    break;
  }
  focusEntry();
}

void PageView::goRelative(int n) {
  int dir = n;
  if (n==1) {
    /* This magic is to deal with continuation pages ("93a"). */
    nextPage();
    return;
  } else if (n==-1) {
    previousPage();
    return;
  }

  int N = tocScene->sheetCount();
  
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
  if (n<=tocScene->sheetCount()) {
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
    gotoTOC();
    break;
  case TOC:
    if (!tocScene->nextSheet())
      gotoEntryPage(1, 1);
    break;
  case Entries:
    if (!entryScene->nextSheet()) 
      gotoEntryPage(currentPage+1, 1); // this may make a new page at the end
    break;
  }
  focusEntry();
}

void PageView::focusEntry() {
  if (currentSection==Entries) {
    if (entryScene->data()->title()->isDefault())
      entryScene->focusTitle();
    else
      entryScene->focusEnd();
  }
}

void PageView::lastPage() {
  gotoEntryPage(book->toc()->newPageNumber()-1);
  entryScene->gotoSheet(entryScene->sheetCount()-1);
  focusEntry();
}

Mode *PageView::mode() const {
  ASSERT(book);
  return book->mode();
}

Notebook *PageView::notebook() const {
  return book;
}

void PageView::wheelEvent(QWheelEvent *e) {
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
  QString newTtl = entryScene->data()->title()->current()->text();
  if (!newTtl.endsWith(QString::fromUtf8(" (cont’d)")))
    newTtl += QString::fromUtf8(" (cont’d)");
  int oldPage = entryScene->startPage() + entryScene->currentSheet();
  int newPage = book->toc()->newPageNumber();
  Style const &style = book->style();

  // Create forward note
  QPointF fwdNotePos(style.real("page-width")/2,
                     style.real("page-height")
                     - style.real("margin-bottom")
                     + style.real("pgno-sep"));
  GfxNoteItem *fwdNote = entryScene->newNote(fwdNotePos);
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
  gotoEntryPage(newPage);
  ASSERT(entryScene);
  // (So now entryScene refers to the new page.)
  entryScene->data()->title()->revise()->setText(newTtl);
  
  // Create reverse note
  QPointF revNotePos(style.real("margin-left"),
                     style.real("margin-top"));
  GfxNoteItem *revNote = entryScene->newNote(revNotePos);
  TextItem *revNoteTI = revNote->textItem();
  cursor = revNoteTI->textCursor();
  QString revNoteText = QString("p. %1 >").arg(oldPage);
  cursor.insertText(revNoteText);
  cursor.setPosition(revNoteText.size()-2);
  revNoteTI->setTextCursor(cursor);
  revNoteTI->tryExplicitLink();
  pp = revNote->mapToScene(revNote->netBounds().topRight());
  revNote->translate(revNotePos - pp);

  gotoEntryPage(oldPage);
  gotoEntryPage(newPage); // pick up new title

  mode()->setMode(Mode::Type);
  entryScene->focusTitle();
}

void PageView::notebookReloaded(QMap<int, int>) {
  setScene(0); // hopefully that avoids crazy UI crashes
  
  delete frontScene;
  frontScene = new FrontScene(book, this);

  delete tocScene;
  tocScene = new TOCScene(book->toc(), this);
  tocScene->populate();
  // simpleNavbar = new SimpleNavbar(tocScene);
  // connect(simpleNavbar, SIGNAL(goRelative(int)), SLOT(goRelative(int)));
  connect(tocScene, SIGNAL(pageNumberClicked(int)),
          SLOT(gotoEntryPage(int)));

  if (entryScene)
    delete entryScene;
  entryScene = 0;

  switch (currentSection) {
  case Front:
    gotoFront();
    break;
  case TOC:
    gotoTOC(currentPage);
    break;
  case Entries:
    gotoEntryPage(currentPage, -1);
    break;
  }
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
      html.add(entryScene);
    }
  }
}
