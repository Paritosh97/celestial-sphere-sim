#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>

#include "typedef.h"

#include "skypoint.h"
#include "linelist.h"

class SkyPoint;

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    auto *geometry = new Qt3DRender::QGeometry();

    // random sky points
    SkyPoint *p[2];
    for(int i = 0; i < 2; i++)
    {
        dms *ra = new dms(10*i);
        dms *dec = new dms(20*i);
        p[i] = new SkyPoint(*ra, *dec);
    }    

    // sending ra and dec
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(double));
    double *ra_dec = reinterpret_cast<double*>(bufferBytes.data());
    *ra_dec++ = p[0]->ra().radians();
    *ra_dec++ = p[0]->dec().radians();
    *ra_dec++ = 0;  // Sending 0 for z cordinate
    *ra_dec++ = p[1]->ra().radians();
    *ra_dec++ = p[1]->dec().radians();
    *ra_dec++ = 0;

    auto *buf = new Qt3DRender::QBuffer(geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setName("pos");
    positionAttribute->setByteStride(3 * sizeof(float));

    geometry->addAttribute(positionAttribute);
    geometry->setBoundingVolumePositionAttribute( positionAttribute );

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    auto *indexBuffer = new Qt3DRender::QBuffer(geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute);

    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.setTitle("Instanced Rendering");
    view.resize(1600, 800);
    view.engine()->qmlEngine()->rootContext()->setContextProperty("_window", &view);
    view.engine()->qmlEngine()->rootContext()->setContextProperty("_instg", geometry);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
