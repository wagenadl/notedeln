// main.C

#include <QApplication>

#include "FilePage.H"
#include "DataBlockText.H"

int main(int argc, char **argv) {
  QApplication qap(argc, argv);
  FilePage *fp = FilePage::create("/tmp/foo.xml");
  DataPage &pg(fp->data());
  pg.setTitle("My Title");
  DataBlockText *blk = dynamic_cast<DataBlockText*>(pg.addBlock("", "textblock"));
  blk->setText("Hello world");
  fp->save();
  return 0;
}


