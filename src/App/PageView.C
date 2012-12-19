// PageView.C

#include "PageView.H"
#include "App.H"
#include "ModSnooper.H"
#include "PageScene.H"
#include "Notebook.H"
#include "PageFile.H"
#include "DataFile.H"
#include "TOCScene.H"
#include "FrontScene.H"
#include "TitleData.H"
#include "GfxLinePalette.H"
#include "GfxMarkPalette.H"
#include "DeletedStack.H"
#include "Assert.H"

#include <QKeyEvent>
#include <QDebug>

PageView::PageView(Notebook *nb, QWidget *parent):
  QGraphicsView(parent), book(nb) {
  deletedStack = new DeletedStack(this);
  frontScene = new FrontScene(nb, this);
  tocScene = new TOCScene(nb->toc(), this);
  tocScene->populate();

  linePalette = new GfxLinePalette();
  linePalette->setParent(this);
  markPalette = new GfxMarkPalette();
  markPalette->setParent(this);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  pageScene = 0;
  currentSection = Front;
}

PageView::~PageView() {
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
  bool take = false;
  if (e->button()==Qt::RightButton) {
    QPointF p = mapToScene(e->pos());
    if (currentSection==Pages) {
      if (e->modifiers() & Qt::ControlModifier) {
	markPalette->letUserChoose(pageScene, p);
	take = true;
      } else if (e->modifiers() & Qt::ShiftModifier) {
	if (linePalette->letUserChoose(pageScene, p))
	  markPalette->setColor(linePalette->color());
	take = true;
      }
    } 
  }
  if (take)
    e->accept();
  else
    QGraphicsView::mousePressEvent(e);
}
  
void PageView::keyPressEvent(QKeyEvent *e) {
  App::instance()->modSnooper()->keyPress(e->key());
  switch (e->key()) {
  case Qt::Key_PageUp:
    previousPage();
    e->accept();
    return;
  case Qt::Key_PageDown:
    nextPage();
    e->accept();
    return;
  case Qt::Key_Home:
    if (e->modifiers() & Qt::ControlModifier) {
      gotoTOC();
      e->accept();
      return;
    }
    break;
  case Qt::Key_End:
    if (e->modifiers() & Qt::ControlModifier) {
      lastPage();
      e->accept();
      return;
    }
    break;
  case Qt::Key_Delete:
    if (currentSection==Pages && pageScene->focusItem()==0) {
      QPointF p = mapToScene(mapFromGlobal(QCursor::pos()));
      QGraphicsItem *gi = pageScene->itemAt(p);
      Item *item;
      while (true) {
	item = dynamic_cast<Item*>(gi);
	if (item)
	  break;
	if (gi)
	  gi = gi->parentItem();
	else
	  break;
      }
      if (item) 
	deletedStack->grabIfRestorable(item);
      e->accept();
      return;
    }
    break;
  case Qt::Key_Insert:
    if (currentSection==Pages && pageScene->focusItem()==0) {
      deletedStack->restoreTop();
      e->accept();
      return;
    }
    break;
  case Qt::Key_P:
    if (e->modifiers() & Qt::ControlModifier) {
      printDialog();
      e->accept();
      return;
    }
  default:
    break;
  }
  QGraphicsView::keyPressEvent(e);
}

void PageView::keyReleaseEvent(QKeyEvent *e) {
  App::instance()->modSnooper()->keyRelease(e->key());
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
      PageFile *file = book->page(te->startPage());
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

  PageFile *file = book->page(te->startPage());
  ASSERT(file);
  ASSERT(file->data());

  leavePage();
  if (pageScene)
    delete pageScene;
  pageScene = 0;

  currentSection = Pages;
  currentPage = n;

  pageScene = new PageScene(file->data(), this);
  pageScene->populate();
  connect(pageScene, SIGNAL(nowOnPage(int)), SLOT(nowOnPage(int)));
  if (book->toc()->isLast(te) && file->data()->isRecent())
    pageScene->makeWritable(); // this should be even more sophisticated
  setScene(pageScene);
  pageScene->gotoSheet(currentPage - te->startPage());
  
  if (file->data()->title()->isDefault())
    pageScene->focusTitle();
  else
    pageScene->focusEnd();
}

void PageView::gotoFront() {
  leavePage();
  currentSection = Front;
  setScene(frontScene);
}

void PageView::gotoTOC(int n) {
  leavePage();
  
  currentSection = TOC;
  currentPage = n;
  setScene(tocScene);
  tocScene->gotoSheet(currentPage-1);
}

void PageView::leavePage() {
  QGraphicsScene *s = scene();
  if (s) {
    QGraphicsItem *fi = s->focusItem();
    if (fi)
      fi->clearFocus(); // this should cause abandon to happen
  }

  if (currentSection==Pages
      && currentPage>1
      && currentPage==book->toc()->newPageNumber()-1) {
    // Leaving the last page in the notebook, not being the only page.
    // If the page is empty, we'll delete it.   
    TOCEntry *te = book->toc()->find(currentPage);
    ASSERT(te);
    PageFile *file = book->page(te->startPage());
    ASSERT(file);
    ASSERT(file->data());
    if (file->data()->isEmpty()) {
      // Leaving an empty page
      if (pageScene)
	delete pageScene;
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
