#include "qt3dwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Application
    QApplication app(argc, argv);

    // Window
    Qt3DWidget *view = new Qt3DWidget();
    view->show();

    return app.exec();
}
