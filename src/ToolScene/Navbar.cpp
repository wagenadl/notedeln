// App/Navbar.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Navbar.C

#include "Navbar.h"
#include "ToolItem.h"
#include "AboutBox.h"
#include "Version.h"

#define NAV_TOC "toc"
#define NAV_FIND "find"
#define NAV_PREV "prev"
#define NAV_NEXT "next"
#define NAV_PLUS "plus"
#define NAV_P10 "p10"
#define NAV_N10 "n10"
#define NAV_END "end"
#define NAV_PRINT "print"
#define NAV_HELP "help"

Navbar::Navbar(QGraphicsItem *parent): Toolbar(parent) {
  disableSelect();
  
  ToolItem *t = 0;
    
  t = new ToolItem();
  t->setSvg(":icons/nav-toc.svg");
  t->setBalloonHelpText(":nav-toc");
  addTool(NAV_TOC, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-p10.svg");
  t->setBalloonHelpText(":nav-p10");
  addTool(NAV_P10, t);
  ti_p10 = t;

  t = new ToolItem();
  t->setSvg(":icons/nav-prev.svg");
  t->setBalloonHelpText(":nav-prev");
  addTool(NAV_PREV, t);
  ti_p1 = t;

  t = new ToolItem();
  t->setSvg(":icons/nav-next.svg");
  t->setBalloonHelpText(":nav-next");
  addTool(NAV_NEXT, t);
  ti_n1 = t;

  t = new ToolItem();
  t->setSvg(":icons/nav-n10.svg");
  t->setBalloonHelpText(":nav-n10");
  addTool(NAV_N10, t);
  ti_n10 = t;

  t = new ToolItem();
  t->setSvg(":icons/nav-end.svg");
  t->setBalloonHelpText(":nav-end");
  addTool(NAV_END, t);
  ti_end = t;

  addSpace(-.5);
  
  t = new ToolItem();
  t->setSvg(":icons/nav-plus.svg");
  t->setBalloonHelpText(":nav-plus");
  addTool(NAV_PLUS, t);
  ti_plus = t;
  //t->setPos(ti_n1->pos());
  //t->setVisible(false);

  addSpace(-1);
  
  t = new ToolItem();
  t->setSvg(":icons/nav-find.svg");
  t->setBalloonHelpText(":nav-find");
  addTool(NAV_FIND, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-print.svg");
  t->setBalloonHelpText(":nav-print");
  addTool(NAV_PRINT, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-help.svg");
  t->setBalloonHelpText(":nav-help");
  addTool(NAV_HELP, t);

}


Navbar::~Navbar() {
}

void Navbar::doLeftClick(QString s, Qt::KeyboardModifiers m) {
  if (s==NAV_TOC)
    emit goTOC(m);
  else if (s==NAV_FIND)
    emit goFind();
  else if (s==NAV_P10)
    emit goRelative(-10, m);
  else if (s==NAV_PREV)
    emit goRelative(-1, m);
  else if (s==NAV_NEXT)
    emit goRelative(1, m);
  else if (s==NAV_N10)
    emit goRelative(10, m);
  else if (s==NAV_END)
    emit goEnd(m);
  else if (s==NAV_PLUS)
    emit goNew(m);
  else if (s==NAV_PRINT)
    emit goPrint();
  else if (s==NAV_HELP)
    showHelp();
}

void Navbar::showHelp() {
  AboutBox::about();
}

void Navbar::setPageType(Navbar::PageType pt) {
  bool isfirst = pt==FrontPage || pt==NoNav;
  bool islast = pt==LastEntry || pt==EmptyLastEntry || pt==NoNav;
  ti_p1->setEnabled(!isfirst);
  ti_p10->setEnabled(!isfirst);
  ti_n1->setEnabled(!islast);
  ti_n10->setEnabled(!islast);
  ti_end->setEnabled(!islast);
}

void Navbar::hidePlus() {
  ti_plus->hide();
}

void Navbar::showPlus() {
  ti_plus->show();
}
