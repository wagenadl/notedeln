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
#include "Notebook.H"
#include "Navbar.H"

PageEditor::PageEditor(Notebook *nb): book(nb) {
  setContentsMargins(0, 0, 0, 0);

  view = new PageView(nb, this);
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
