#include "customwindow.h"
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    // Application
    QApplication app(argc, argv);

    // Window
    CustomWindow *view = new CustomWindow();
    view->setWindowState(Qt::WindowMaximized);
    view->show();

    return app.exec();
}
