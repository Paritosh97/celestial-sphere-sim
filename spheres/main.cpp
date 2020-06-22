#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>

#include "typedef.h"

#include "instancedgeometry.h"

#include "skypoint.h"
#include "linelist.h"

class SkyPoint;

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QVector<QVector3D> pos;
    for(int i = 1; i < 10; i++)
    {
        dms *ra = new dms(i*36);
        dms *dec = new dms(i*30);
        std::shared_ptr<SkyPoint>  point(new SkyPoint(*ra, *dec));

        pos.append(QVector3D(point->ra().radians(), point->dec().radians(), 0));
    }
    qDebug()<<pos;

    InstancedGeometry instGeom;
    instGeom.setPoints(pos);

    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.setTitle("Instanced Rendering");
    view.resize(1600, 800);
    view.engine()->qmlEngine()->rootContext()->setContextProperty("_window", &view);
    view.engine()->qmlEngine()->rootContext()->setContextProperty("_instg", &instGeom);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
