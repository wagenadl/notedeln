// TOCScene.C

#include "TOCScene.H"
#include "TOC.H"

TOCScene::TOCScene(TOC *data, QObject *parent):
  BaseScene(data, parent),
  data(data) {
}

TOCScene::~TOCScene() {
}

void TOCScene::populate() {
  BaseScene::populate();
  rebuild();
}

QString TOCScene::title() const {
  return "Table of Contents";
}

void TOCScene::rebuild() {
}
