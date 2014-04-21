// App/PageEditor.cpp - This file is part of eln

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

// PageEditor.C

#include "PageEditor.H"
#include "PageView.H"
#include "ToolView.H"
#include "Toolbars.H"
#include "SceneBank.H"
#include "Notebook.H"
#include "Navbar.H"

#include <QDebug>
#include <QKeyEvent>

PageEditor::PageEditor(SceneBank *bank): bank(bank) {
  Notebook *nb = bank->book();
  setContentsMargins(0, 0, 0, 0);
  setAttribute(Qt::WA_DeleteOnClose, true);

  QString ttl = nb->bookData()->title();
  QString appname = "eln";
#ifndef QT_NO_DEBUG
  appname += " (debug vsn)";
#endif
  setWindowTitle(ttl.replace(QRegExp("\\s\\s*"), " ") + " - " + appname);

  view = new PageView(bank, this);
  toolview = new ToolView(nb->mode(), view);

  connect(toolview->toolbars()->navbar(), SIGNAL(goTOC()),
	  view, SLOT(gotoTOC()));
  connect(toolview->toolbars()->navbar(), SIGNAL(goFind()),
	  view, SLOT(openFindDialog()));
  connect(toolview->toolbars()->navbar(), SIGNAL(goEnd()),
	  view, SLOT(lastPage()));
  connect(toolview->toolbars()->navbar(), SIGNAL(goRelative(int)),
	  view, SLOT(goRelative(int)));

  connect(view, SIGNAL(onEntryPage(int, int)),
	  toolview->toolbars(), SLOT(showTools()));
  connect(view, SIGNAL(onFrontMatter(int)),
	  toolview->toolbars(), SLOT(hideTools()));
  connect(view, SIGNAL(scaled(double)),
	  toolview, SLOT(setScale(double)));
   
  toolview->setGeometry(0, 0, width(), height());
  setCentralWidget(view);
  view->gotoFront();  
}

PageEditor::~PageEditor() {
}

void PageEditor::resizeEvent(QResizeEvent *e) {
  QMainWindow::resizeEvent(e);
  toolview->setGeometry(0, 0, width(), height());
}

void PageEditor::keyPressEvent(QKeyEvent *e) {
  bool take = false;
  switch (e->key()) {
  case Qt::Key_F12:
    {
      PageEditor *editor = new PageEditor(bank);
      editor->resize(size());
      switch (view->section()) {
      case PageView::Front:
	break;
      case PageView::TOC:
	editor->gotoTOC(view->pageNumber());
	break;
      case PageView::Entries:
	editor->gotoEntryPage(view->pageName());
	break;
      }
      editor->show();
      take = true;
    }
    break;
  default:
    break;
  }

  if (take)
    e->accept();
  else
    QMainWindow::keyPressEvent(e);
}

void PageEditor::gotoEntryPage(QString s) {
  view->gotoEntryPage(s);
}

void PageEditor::gotoTOC(int n) {
  view->gotoTOC(n);
}

void PageEditor::gotoFront() {
  view->gotoFront();
}
