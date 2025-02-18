#include <QApplication>
#include "satellite.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  Satellite window;
  window.show();
  return QApplication::exec();
}
